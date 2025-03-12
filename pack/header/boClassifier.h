#ifndef _GROUNDNUT_BOCLASSIFIER_H_
#define _GROUNDNUT_BOCALSSIFIER_H_

#include "burstClassifier.h"
#include "kburst.h"
#include "utils-metric.h"
#include <sstream>

namespace groundnut
{

struct ReviewBurst
{
    std::shared_ptr<KBurst> testBurst;
    SearchResult prediction;
    SearchResult searchResult;
    float score;
    bool isWrong = false;
    bool isFullyCorrect = false;

    void UpdateFlags();

    inline std::string ToString()
    {
        std::stringstream ss;
        ss << "testBurst: "  << testBurst->ToString() << "\n";
        ss << "prediction: " << prediction.ToString() << "\n";
        ss << "same label search:" << searchResult.ToString() << "\n";
        ss << "score:" << score << "\n";
        ss << "isWrong: " << isWrong << " isFullyCorrect: " << isFullyCorrect << "\n";
        return ss.str();
    }
};

struct ReviewBook
{
    std::vector<std::vector<ReviewBurst>> reviewBook;

    inline std::string ToString()
    {
        std::stringstream ss;

        for(auto& reviewInstance : reviewBook)
        {
            for(auto& reviewBurst : reviewInstance)
            {
                ss << reviewBurst.ToString() << std::endl;
            }
        }

        return ss.str();
    }
};

class Aggregator
{
friend class BoClassifier;
public:
    Aggregator() = default;
    float AddBurstPrediction(std::shared_ptr<KBurst> pBurst, SearchResult prediction, bool reviewEnable = true);
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
    std::vector<ReviewBurst> Predict(BurstVec instance, std::string& strResult, bool reviewEnable = true);
    ReviewBook Predict(std::unordered_map<uint16_t, BurstGroups>* testset, ClassificationMetrics& metric, bool reviewEnable = true);

private:
    BurstClassifier bclf;

    int maxUniPkt = 50;
};


}

#endif