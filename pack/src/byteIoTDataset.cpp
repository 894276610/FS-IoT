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

void ByteIoTDataset::TrainTestSplit()
{
    float trainRate = config.trainRate;

    for (auto& [deviceId, instances] : rawMap)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (*a) < (*b);
            });
        }
        
        size_t trainNum = std::ceil(instances.size() * trainRate);
        size_t testStart = std::ceil(instances.size() * (1- config.testRate));

        BurstVec trainInstances(instances.begin(), instances.begin() + trainNum);
        BurstVec testInstances(instances.begin() + testStart, instances.end());

        trainset.insert({ deviceId, trainInstances });
        testset.insert({ deviceId, testInstances });
    }
}
}