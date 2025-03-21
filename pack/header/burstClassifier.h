#ifndef  _GROUNDNUT_BURSTCLASSIFIER_H_
#define _GROUNDNUT_BURSTCLASSIFIER_H_

#include <iostream>
#include <unordered_map>
#include "kburst.h"

namespace groundnut{

struct ConfigBurstClf
{
    int uniPktTolr = 10;
    int pktTolr = 100;
    int maxUniPkt = 50;
    int maxPktIndex = 500;
    int maxTrainItem = 1000;
    float distanceTrh = 0.5;
    float penalty = 0.01;


    std::string ToString() const
    {
        std::stringstream ss;
        ss << "(uniPkTolr=" << uniPktTolr << ")";
        ss << "(pkTolr=" << pktTolr << ")";
        ss << "(maxUniPkt=" << maxUniPkt << ")";
        ss << "(maxPktIndex=" << maxPktIndex << ")";
        ss << "(distanceTrh=" << distanceTrh << ")";
        ss << "(maxTrainIt=" << maxTrainItem << ")";
        ss << "(penalty=" << penalty << ")";
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