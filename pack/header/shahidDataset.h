#ifndef _SHAHID_DATASET_H_
#define _SHAHID_DATASET_H_

#include <string>
#include <vector>
#include <mlpack.hpp>
#include <unordered_map>

#include "kdevice.h"
#include "packetDataset.h"

namespace groundnut
{

class ShahidConfig
{
public:
    size_t configN = 6;
    size_t timeout = 600;

    std::string ToString()
    {
        return "(configN=" + std::to_string(configN) + ")(timeout=" + std::to_string(timeout) + ")";
    }
};


class ShahidStream
{
public:
    size_t devId;
    uint32_t streamId; // hash5tuple
    timespec startTime{0,0};
    size_t configN; // N packets to collect each direction.
    std::vector<const KPacket*> packets;
    arma::colvec features;

    ShahidStream(size_t devId, uint32_t streamId, timespec startTime, size_t configN):
    devId(devId), streamId(streamId), startTime(startTime), configN(configN){}

    void AddPacket(const KPacket* packet);
    void FeatureExtraction();
};

class ShahidSlot
{
public:
    size_t deviceId;
    size_t slotId;
    size_t configN;
    std::vector<std::shared_ptr<ShahidStream>> streams;

public:
    ShahidSlot(size_t deviceId, size_t slotId, size_t configN):
    deviceId(deviceId), slotId(slotId), configN(configN){}
    void AddPacket(const KPacket* packet);
    void FeatureExtraction();
};

typedef std::vector<ShahidSlot> ShahidSlots;

class ShahidDataset
{

public:
    ShahidDataset(DatasetEnum name, size_t configN, size_t timeSlot, size_t timeout=600): 
    name(name), configN(configN), slotDuration(timeSlot), timeout(timeout)
    {}

    void Load(PacketDataset& dataset);
    void TrainTestSplit(std::unordered_map<uint16_t, ShahidSlots>& trainSet, std::unordered_map<uint16_t, ShahidSlots>& testSet, int minute); // TODO

    std::unordered_map<uint16_t, ShahidSlots>& GetData(){return data;}
    
public:
    DatasetEnum name;
    size_t configN; // first N value of the stream.
    size_t slotDuration = 1800;
    size_t timeout; // timeout value of a long stream.
    float trainRate = 0.5;
    float testRate = 0.5;

    std::vector<groundnut::KDevice> devicesVec; 

private:
    void AddPacket(const KPacket* packet);
    void FeatureExtraction();

private:
    std::unordered_map<uint16_t, ShahidSlots> data;

};

}

#endif
