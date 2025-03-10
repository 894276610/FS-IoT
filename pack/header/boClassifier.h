#ifndef _GROUNDNUT_BOCLASSIFIER_H_
#define _GROUNDNUT_BOCALSSIFIER_H_

#include "burstClassifier.h"
#include "kburst.h"
#include "utils-metric.h"

namespace groundnut
{

class ReviewBurst
{
    std::shared_ptr<KBurst> testBurst;
    BurstPrediction burstPrediction;
    std::shared_ptr<KBurst> positiveBurst;
    float positiveDistance;
    std::string comment;
    float score;
};

class ReviewInstance
{
    std::vector<ReviewBurst> reviewList;
};

class Aggregator
{
public:
    Aggregator() = default;
    void AddBurstPrediction(std::shared_ptr<KBurst> pBurst, BurstPrediction prediction);
    std::string FetchResult();

private:
    std::unordered_map<std::string, float> scoreBoard;
    std::set<std::string> devNameSet;

};

class BoClassifier
{

public: 
    BoClassifier(int maxUniPkt):maxUniPkt(maxUniPkt), bclf(maxUniPkt){}
    void Train(std::unordered_map<uint16_t, BurstGroups>* trainset);
    std::string Predict(BurstVec instance);
    ClassificationMetrics Predict(std::unordered_map<uint16_t, BurstGroups>* testset);

private:
    BurstClassifier bclf;

    int maxUniPkt = 50;
};


}

#endif