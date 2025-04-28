#include "shahidDataset.h"
#include "time-utils.h"
#include "timer.h"

namespace groundnut
{

void ShahidDataset::TrainTestSplit(std::unordered_map<uint16_t, ShahidSlots>& trainSet, std::unordered_map<uint16_t, ShahidSlots>& testSet, int trainBudget)
{
    for (auto& [deviceId, instances] : this->data)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (a.slotId) < (b.slotId);
            });
        }
        
        size_t totalSize = instances.size();

        int maxTrainNum = std::ceil(totalSize * trainRate);
      
        int trainSlotBudget =   trainBudget * 60 / slotDuration;
        size_t trainNum = std::min(maxTrainNum, trainSlotBudget);

        size_t testNum = std::ceil(totalSize * testRate);
        size_t testStart = instances.size() - testNum;

        ShahidSlots trainInstances(instances.begin(), instances.begin() + trainNum);
        ShahidSlots testInstances(instances.begin() + testStart, instances.end());

        trainSet.insert({ deviceId, trainInstances });
        testSet.insert({  deviceId, testInstances });

    }
}

void ShahidDataset::FeatureExtraction()
{
    for(auto& [devId, slots] : data)
    {
        for(auto& slot : slots)
        {
            slot.FeatureExtraction();
        }
    }
}
void ShahidSlot::FeatureExtraction()
{
    for(auto& stream: streams)
    {
        stream->FeatureExtraction();
    }
}

void ShahidStream::FeatureExtraction()
{
    size_t inIndex = 0;
    size_t outIndex = 0;
    timespec prevInTime{0,0};
    timespec prevOutTime{0,0};

    if(configN <=0)
    {
        std::cout << "configN:" << configN  << " should be larger than 0. "<< std::endl;
    }
    features.resize( 4*configN - 2);
    features.fill(0);

    // transform packets into the features.
    for(const KPacket* packet : packets)
    {
        if((inIndex > configN ) || (outIndex > configN ))
        {
            std::cout << "Fault! index:" << inIndex << "." << outIndex << "." << " exceed the configN" << std::endl;
        }

        if(inIndex == configN && outIndex == configN)
        {
            break;
        }

        if(packet->signedLen < 0 && inIndex == configN)
        {
            continue;
        }

        if(packet->signedLen > 0 && outIndex == configN)
        {
            continue;
        }

        if(packet->signedLen < 0) // inpacket 0--- N-1, N + 0 --- N + N-2
        {
            features.at(inIndex) = - packet->signedLen;
            
            if(inIndex++)
            {
                // not the first input packet
                features.at(configN + inIndex - 2) = Time2Float(packet->timestamp - prevInTime);
                prevInTime = packet->timestamp;
            }
            else
            {
                prevInTime = packet->timestamp; 
                continue;
            }         
        }
        else //  outpacket 2N - 1 --- 2N + N - 2 , 3N - 1 --- 3N + N - 3
        {
            features.at( 2 * configN + outIndex - 1) = packet->signedLen;

            if(outIndex++)
            {
                features.at( 3 * configN + outIndex - 3) = Time2Float(packet->timestamp - prevOutTime);
                prevOutTime = packet->timestamp;  
            }
            else
            {
                prevOutTime = packet->timestamp;
                continue;
            }
        }
    }
}

void ShahidStream::AddPacket(const KPacket* packet)
{
    this->packets.push_back(packet);
}

void ShahidSlot::AddPacket(const KPacket* packet)
{
    // 1. find if has the same stream 
    auto stream = std::find_if(this->streams.begin(), this->streams.end(), [&](std::shared_ptr<ShahidStream> stream){
        
        if(stream->streamId != packet->hash5tuple)
            return false;
        
        if(packet->timestamp < stream->startTime)
            return false;
        
        if((packet->timestamp - stream->startTime) > timespec{600,0})
            return false;
        
        if((packet->timestamp - stream->startTime) < timespec{600,0})
            return true;

        return false;
    });

    if(stream != this->streams.end())
    {
        // find and append.
        (*stream)->AddPacket(packet);
    }
    else
    {
        // create another stream
        this->streams.push_back(std::make_shared<ShahidStream>(packet->deviceId, packet->hash5tuple, packet->timestamp, configN));
        this->streams.back()->AddPacket(packet);
    }
}

void ShahidDataset::Load(PacketDataset& dataset)
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
        AddPacket(&packet);
    }

    FeatureExtraction();
}

// Assume the packets are in time order.
void ShahidDataset::AddPacket(const KPacket* packet)
{
    // find the corresponding shahid slot and insert it.
    if(packet->hash5tuple == 0)
    {
        return;
    }
    
    uint16_t deviceId = packet->deviceId;
    if(this->data.find(deviceId) == data.end())
    {
        this->data.insert({deviceId, ShahidSlots()});
    }

    ShahidSlots& slots = this->data[deviceId];
    size_t currentSlot = packet->timestamp.tv_sec / this->slotDuration;
    if(slots.size() == 0 || slots.back().slotId != currentSlot)
    {
        slots.push_back(ShahidSlot(deviceId, currentSlot, configN));
    }

    slots.back().AddPacket(packet);
}

}