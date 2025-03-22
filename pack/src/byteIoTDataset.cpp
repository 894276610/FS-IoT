#include "byteIoTDataset.h"
#include <cmath>

namespace groundnut{

void ByteIoTDataset::Load(PacketDataset& dataset)
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
        time_t slotId = packet.timestamp.tv_sec / config.slotDuration;

        AddPacket(packet.deviceId, slotId, &packet);
    }

    MakeInstances();
}

void ByteIoTDataset::AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet)
{
    auto& mutex = mapDevMutexes[deviceId % kDeviceMutexCount];
    std::unique_lock<std::mutex> lock(mutex);
    mapByDevTime[deviceId][slotId].push_back(packet);
    timeSlots.insert(slotId);
}

void ByteIoTDataset::MakeInstances()
{
    for (time_t slotId : timeSlots) {
        for (auto& [deviceId, slotMap] : mapByDevTime) {

            std::vector<const KPacket*> pktVec;

            auto it = slotMap.find(slotId);
            if (it == slotMap.end() || it->second.empty()) {
                continue;
            }

            pktVec = std::move(it->second);
            slotMap.erase(it);
            
            std::shared_ptr<KBurst> pInstance = std::make_shared<KBurst>(devicesVec[deviceId]);
            for(auto* packet : pktVec)
            {
                pInstance->AddPacket(*packet);
            }
            
            rawMap[deviceId].emplace_back(pInstance);
        }
    }
}

// should not exceed the bounded training rate(e.g. 30%)
// should not exceed the bounded instances min15x * 15 * 60/  1800
float ByteIoTDataset::TrainTestSplitByTime(int min15x)
{
    float averageTrainInstanceSize = 0;
    for (auto& [deviceId, instances] : rawMap)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (*a) < (*b);
            });
        }
        
        size_t totalSize = instances.size();

        std::cout << "device: " + GetDevicesVec()[deviceId].GetLabel() << std::endl;

        int maxTrainNum = std::ceil(totalSize * config.trainRate);
        std::cout << "totalSize:" << totalSize << "trainRate" << config.trainRate;
        std::cout << "maxTrainNum:" << maxTrainNum << std::endl;

        int min15xPreferTrainNum = min15x * 15 * 60 / config.slotDuration;
        size_t trainNum = std::min(maxTrainNum, min15xPreferTrainNum);

        size_t testNum = std::ceil(totalSize * config.testRate);
        size_t testStart = instances.size() - testNum;

        BurstVec trainInstances(instances.begin(), instances.begin() + trainNum);
        BurstVec testInstances(instances.begin() + testStart, instances.end());

        trainset.insert({ deviceId, trainInstances });
        testset.insert({  deviceId, testInstances });

        averageTrainInstanceSize += trainNum;
    }
    averageTrainInstanceSize /= rawMap.size();
    return averageTrainInstanceSize;
}
// return average train instance size (per device)
float ByteIoTDataset::TrainTestSplit()
{
    float averageTrainInstanceSize = 0;
    for (auto& [deviceId, instances] : rawMap)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (*a) < (*b);
            });
        }
        
        size_t totalSize = instances.size();

        // std::cout << "device: " + GetDevicesVec()[deviceId].GetLabel() << std::endl;
        // std::cout << "totalSize:" << totalSize<< std::endl;
        size_t trainNum = std::ceil(totalSize * config.trainRate);
        int valiNum = std::floor(totalSize * config.valiRate);
        size_t testNum = std::ceil(totalSize * config.testRate);

        if(trainNum + valiNum + testNum > instances.size())
        {
            std::cout <<"overflow ! totalSize:" + std::to_string(totalSize) + "," \
            + std::to_string(trainNum + valiNum + testNum)\
            + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n" << std::endl;

            throw "overflow ! totalSize:" + std::to_string(totalSize) + "," \
            + std::to_string(trainNum + valiNum + testNum)\
            + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        if( valiNum <=0 )
        {
            std::cout << "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() << std::endl;
            throw "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        size_t testStart = instances.size() - testNum;

        BurstVec trainInstances(instances.begin(), instances.begin() + trainNum);
        BurstVec testInstances(instances.begin() + testStart, instances.end());

        trainset.insert({ deviceId, trainInstances });
        testset.insert({ deviceId, testInstances });

        averageTrainInstanceSize += trainNum;
        // std::cout << "trainNum:" << trainNum << std::endl;
    }

    averageTrainInstanceSize /= rawMap.size();

    return averageTrainInstanceSize;
}

}