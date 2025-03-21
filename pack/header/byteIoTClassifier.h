#ifndef _GROUNDNUT_BYTEIOTCLASSIFIER_H_
#define _GROUNDNUT_BYTEIOTCLASSIFIER_H_

#include "burstClassifier.h"
#include "boClassifier.h"

namespace groundnut{

class ByteIoTClassifier
{

public:
ByteIoTClassifier() = default;

void Train(std::unordered_map<uint16_t, BurstVec>* trainset);
std::string Predict(const std::shared_ptr<KBurst> instance);
void Predict(std::unordered_map<uint16_t, BurstVec>* testset, ClassificationMetrics& metric, bool reviewEnable);


private:
std::unordered_map<uint16_t, BurstVec>* oriTrainset;

};

    
}

#endif