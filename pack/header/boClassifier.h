#ifndef _GROUNDNUT_BOCLASSIFIER_H_
#define _GROUNDNUT_BOCLASSIFIER_H_

#include "burstClassifier.h"
#include "kburst.h"
#include "resultBundle.h"
#include <sstream>
#include <memory>
#include <vector>


namespace groundnut
{

struct ReviewBurst
{
    std::shared_ptr<KBurst> testBurst;
    SearchResult prediction;
    SearchResult searchResult;
    float score = 0;
    bool isWrong = false;
    bool isFullyCorrect = false;

    inline void UpdateFlags();

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
        ss << "-----------------\n";

        return ss.str();
    }

    inline void Tofile(const std::filesystem::path path)
    {
        std::ofstream fout(path);
        fout << ToString();
        fout.close();
    }
};

class Aggregator
{
friend class BoClassifier;
public:
    Aggregator(float distance_threshold=0.5, float penalty=0.01): 
    distance_threshold(distance_threshold), penalty(penalty){};
    float AddBurstPrediction(std::shared_ptr<KBurst> pBurst, SearchResult prediction, bool reviewEnable = true);
    std::string FetchResult();

private:
    std::unordered_map<std::string, float> scoreBoard;
    std::set<std::string> devNameSet;
    float distance_threshold =0.45;
    float penalty = 0.01;
};

class BoClassifier
{

public: 
    BoClassifier(const FsClfConfig& config): bclf(config){}
    void Train(std::unordered_map<uint16_t, BurstGroups>* trainset);
    std::vector<ReviewBurst> Predict(BurstVec instance, std::string& strResult, bool reviewEnable = true);
    ReviewBook Predict(std::unordered_map<uint16_t, BurstGroups>* testset, ResultBundle& result, bool reviewEnable = true);

private:
    BurstClassifier bclf;
    FsClfConfig config;
};

}
#endif