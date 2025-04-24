#ifndef  _GROUNDNUT_BURSTCLASSIFIER_H_
#define  _GROUNDNUT_BURSTCLASSIFIER_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include "kburst.h"
#include <sstream>

namespace groundnut{

struct ConfigBurstClf
{
    int uniPktTolr = 200; // 200 for lsenable = true. 10 for lsenable = false;
    int pktTolr = 1000;   // 1000 for lsenable = false; 100 for lsenable = true
    int maxUniPkt = 1000; // duplicated with configburstdataset
    int maxPktIndex = 500; 
    int maxTrainItem = 1000;
    float distanceTrh = 0.45;
    float penalty = 0.01;

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "(Tuni=" << uniPktTolr << ")";
        ss << "(Tpkt=" << pktTolr << ")";
        ss << "(mUniPkt=" << maxUniPkt << ")";
        ss << "(mPktIdx=" << maxPktIndex << ")";
        ss << "(dTrh=" << distanceTrh << ")";
        ss << "(maxTrainIt=" << maxTrainItem << ")";
        ss << "(pen=" << penalty << ")";
        return ss.str();
    }
};

class BurstClassifier{

public:
BurstClassifier(const ConfigBurstClf& config):config(config)
{
    for (int i = 0; i < config.maxUniPkt; ++i) {
        uniPktMutex.emplace_back(std::make_unique<std::mutex>());
    }

    std::vector<BurstVec> temp(config.maxPktIndex);
	train.resize(config.maxUniPkt, temp);

}

void Train(std::unordered_map<uint16_t, BurstGroups>* trainset);
SearchResult Predict(const std::shared_ptr<KBurst> burst);
SearchResult ReviewSearch(const std::shared_ptr<KBurst> burst);

private:

void TrainDevice(uint16_t deviceId, BurstGroups burstGroups);
std::unordered_map<std::shared_ptr<KBurst>, int> MergeByHash(BurstGroups& burstGroups);
void MergeByKey(std::unordered_map<std::shared_ptr<KBurst>, int>* map);

std::shared_ptr<KBurst> FindDistanceLT(
    std::unordered_map<std::shared_ptr<KBurst>, int>* map,
    std::shared_ptr<KBurst> burst, float threshold);

std::shared_ptr<KBurst> FindKeysEQ(
    const std::unordered_map<std::shared_ptr<KBurst>, int>* map,
    const std::shared_ptr<KBurst>& burst);

private:

ConfigBurstClf config;
BurstCache cache;
std::vector<BurstGroups> train;
std::vector<std::unique_ptr<std::mutex>> uniPktMutex;
std::unordered_map<uint16_t, BurstGroups>* oriTrainset;

};
}

#endif