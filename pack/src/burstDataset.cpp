#include "burstDataset.h"
#include <cmath>
#include "kburst.h"

namespace groundnut{
             
void BurstDataset::Load(PacketDataset& dataset)
{  
    // name 
    this->name = dataset.GetName();

    // device migration
    devicesVec.clear();
    devicesVec.resize(dataset.GetDevicesMap().size());
    
    for(auto [mac_string, kdevice] : dataset.GetDevicesMap())
    {
        devicesVec[kdevice.GetDevId()] = kdevice;
    }
    
    // packet migration
    for (const groundnut::KPacket& packet : dataset.GetDataset())
    {
        time_t slotId = packet.timestamp.tv_sec / configBurstDataset.slotDuration;

        AddPacket(packet.deviceId, slotId, &packet);
    }

    MakeBursts();
}

void BurstDataset::AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet)
{
    auto& mutex = mapDevMutexes[deviceId % kDeviceMutexCount];
    std::unique_lock<std::mutex> lock(mutex);
    mapByDevTime[deviceId][slotId].push_back(packet);
    timeSlots.insert(slotId);
}

void BurstDataset::MakeBursts()
{
    for (time_t slotId : timeSlots) {
        for (auto& [deviceId, slotMap] : mapByDevTime) {

            std::vector<const KPacket*> pktVec;

            { // 临界区：快速提取数据并清理条目
                auto& mapMutex = this->mapDevMutexes[deviceId % kDeviceMutexCount];
                std::unique_lock<std::mutex> lock(mapMutex);
                auto it = slotMap.find(slotId);
                if (it == slotMap.end() || it->second.empty()) {
                    continue;
                }
                pktVec = std::move(it->second);
                slotMap.erase(it);
            } // 释放锁
            
            BurstMaker maker(&pktVec, devicesVec[deviceId], configBurstDataset.burstTrh);
            BurstVec burstBlock = maker.MakeBursts();

            { // 临界区：线程安全地添加结果
                auto& blockMutex = blockMutexes[deviceId % kDeviceMutexCount];
                std::unique_lock<std::mutex> lock(blockMutex);
                rawMap[deviceId].emplace_back(std::move(burstBlock));
            }
        }
    }

    //mapByDevTime.clear();
}

float BurstDataset::TrainTestSplitByTime(int min)
{
    float avgInstance = 0;

    for (auto& [deviceId, burstGroup] : rawMap)
    {
        std::sort(burstGroup.begin(), burstGroup.end(), [](const BurstVec& a, const BurstVec& b) {
            return (*a.front()) < (*b.front());
        });   
        
        size_t totalSize = burstGroup.size();
        
        int minBudgetTrainNum = min * 60 / configBurstDataset.slotDuration;
        int maxTrainNum = std::ceil(totalSize * configBurstDataset.trainRate);
        size_t trainNum = std::min(maxTrainNum, minBudgetTrainNum);

        std::cout << "totalSize:" << totalSize << "trainNum:" << trainNum << std::endl;

        int valiNum = std::floor(totalSize * configBurstDataset.valiRate);
        size_t testNum = std::ceil(totalSize * configBurstDataset.testRate);

        size_t testStart = totalSize - testNum;
        size_t valiStart = totalSize - testNum - valiNum;

        if(trainNum + testNum + valiNum > burstGroup.size())
        {
            valiNum = burstGroup.size() - trainNum - testNum;
            throw "overflow ! totalNum:" + std::to_string(burstGroup.size())\
             + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        if( valiNum <=0 )
        {
            throw "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        BurstGroups trainGroups(burstGroup.begin(), burstGroup.begin() + trainNum);
        BurstGroups valiGroups(burstGroup.begin() + valiStart, burstGroup.begin() + valiStart + valiNum);
        BurstGroups testGroups(burstGroup.begin() + testStart, burstGroup.end());

        trainset.insert({ deviceId, trainGroups });
        valiset.insert({ deviceId, valiGroups });
        testset.insert({ deviceId, testGroups });
        
        avgInstance += trainNum;
    }

    avgInstance /= trainset.size();
    return avgInstance;
}

void BurstDataset::TrainTestSplit()
{
    for (auto& [deviceId, burstGroup] : rawMap)
    {
        {
            std::stable_sort(burstGroup.begin(), burstGroup.end(), [](const BurstVec& a, const BurstVec& b) {
                return (*a.front()) < (*b.front());
            });
        }
        
        size_t trainNum = std::ceil(burstGroup.size() * configBurstDataset.trainRate);
        size_t testNum = std::ceil(burstGroup.size() * configBurstDataset.testRate);
        int valiNum = std::floor(burstGroup.size() * configBurstDataset.valiRate);
        
        if(trainNum + testNum + valiNum > burstGroup.size())
        {
            valiNum = burstGroup.size() - trainNum - testNum;
            throw "overflow ! totalNum:" + std::to_string(burstGroup.size())\
             + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        if( valiNum <=0 )
        {
            throw "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }
        
        size_t testStart = burstGroup.size() - testNum;
        size_t valiStart = burstGroup.size() - testNum - valiNum;

        BurstGroups trainGroups(burstGroup.begin(), burstGroup.begin() + trainNum);
        BurstGroups valiGroups(burstGroup.begin() + valiStart, burstGroup.begin() + valiStart + valiNum);
        BurstGroups testGroups(burstGroup.begin() + testStart, burstGroup.end());

        trainset.insert({ deviceId, trainGroups });
        valiset.insert({ deviceId, valiGroups });
        testset.insert({ deviceId, testGroups });
    }
}

}