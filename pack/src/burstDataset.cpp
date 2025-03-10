#include "burstDataset.h"
#include <cmath>
#include "kburst.h"

namespace groundnut{
             
    
void BurstDataset::Load(PacketDataset& dataset, const BurstTrh& trh)
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
        time_t slotId = packet.timestamp.tv_sec / slotDuration;

        AddPacket(packet.deviceId, slotId, &packet);
    }

    this->trh = trh;
    MakeBursts(trh);
}

void BurstDataset::AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet)
{
    auto& mutex = mapDevMutexes[deviceId % kDeviceMutexCount];
    std::unique_lock<std::mutex> lock(mutex);
    mapByDevTime[deviceId][slotId].push_back(packet);
    timeSlots.insert(slotId);
}

void BurstDataset::MakeBursts(const BurstTrh& trh)
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
            
            BurstMaker maker(&pktVec, devicesVec[deviceId], trh);
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

void BurstDataset::TrainTestSplit(float trainRate)
{
    for (auto& [deviceId, burstGroup] : rawMap)
    {
        {
            std::stable_sort(burstGroup.begin(), burstGroup.end(), [](const BurstVec& a, const BurstVec& b) {
                return (*a.front()) < (*b.front());
            });
        }
        
        size_t trainNum = std::ceil(burstGroup.size() * trainRate);

        BurstGroups trainGroups(burstGroup.begin(), burstGroup.begin() + trainNum);
        BurstGroups testGroups(burstGroup.begin() + trainNum, burstGroup.end());

        trainset.insert({ deviceId, trainGroups });
        testset.insert({ deviceId, testGroups });
    }
}
}