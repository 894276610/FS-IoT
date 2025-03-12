#ifndef  _GROUNDNUT_BURSTCLASSIFIER_H_
#define _GROUNDNUT_BURSTCLASSIFIER_H_

#include <iostream>
#include <unordered_map>
#include "kburst.h"

namespace groundnut{

class BurstClassifier{

public:
BurstClassifier(int maxUniPkt)
{
    for (int i = 0; i < maxUniPkt; ++i) {
        uniPktMutex.emplace_back(std::make_unique<std::mutex>());
    }

    std::vector<BurstVec> temp(500);
	train.resize(maxUniPkt, temp);

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

BurstCache cache;
std::vector<BurstGroups> train;
std::vector<std::unique_ptr<std::mutex>> uniPktMutex;
std::unordered_map<uint16_t, BurstGroups>* oriTrainset;

};
    
}

#endif