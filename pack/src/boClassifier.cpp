#include "boClassifier.h"
#include <set>
#include <cmath>
#include "resultBundle.h"
#include <thread>
#include "timer.h"

namespace groundnut{

void ReviewBurst::UpdateFlags()
{   
    const auto& bursts = prediction.nearTrainBursts;

    if(bursts.empty())
    {
        isWrong = isFullyCorrect = false;
        return;
    }

    if(bursts.size() == 1)
    {
        isFullyCorrect = bursts.front()->GetLabel() == testBurst->GetLabel();
        isWrong = !isFullyCorrect;
        return;
    }

    isFullyCorrect = false;
    const auto label_match = [this](const auto& burst)
    {
        return burst->GetLabel() == testBurst->GetLabel();
    };

    isWrong = std::none_of(bursts.begin(), bursts.end(), label_match);
}

float Aggregator::AddBurstPrediction(std::shared_ptr<KBurst> pBurst, SearchResult prediction, bool reviewEnable)
{
    this->devNameSet.clear();
    
    // predict names
    for(auto& pburst : prediction.nearTrainBursts)
    {
        this->devNameSet.insert(pburst->GetLabel());
    }

    // calculate score
    float score =  log(pBurst->GetUniPktNum() + 1) / (devNameSet.size() * (sqrt(prediction.minDistance) + 1));

    if(prediction.minDistance > distance_threshold || pBurst->GetPktNum() < 2)
    {
        score*= penalty;
    }

    // add to scoreBoard
    for(const auto& name: devNameSet)
    {
        auto [it, inserted] = scoreBoard.try_emplace(name, score);
        
        if(!inserted)
        {
            it->second += score;
        }
    }

    return score;
}

std::string Aggregator::FetchResult()
{
    const std::vector<std::pair<std::string, float>> vec{scoreBoard.begin(), scoreBoard.end()};

    const auto max_it = std::max_element(vec.begin(), vec.end(),
    [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    return vec.empty() ? "unknown" : max_it->first;
}

void BoClassifier::Train(std::unordered_map<uint16_t, BurstGroups>* trainset)
{
    PROFILE_SCOPE("train", config.ToString().c_str());
    bclf.Train(trainset);
}

std::vector<ReviewBurst> BoClassifier::Predict(BurstVec instance, std::string& strResult, bool reviewEnable)
{
    Aggregator agt(config.distanceTrh, config.penalty);
    std::vector<ReviewBurst> reviewList;

    for(auto& pBurst: instance)
    {
        const SearchResult& prediction = bclf.Predict(pBurst);
        const float score = agt.AddBurstPrediction(pBurst, prediction);

        if(!reviewEnable) continue;

        // review logic
        ReviewBurst reviewBurst;    
        reviewBurst.testBurst = pBurst;
        reviewBurst.prediction = prediction;
        reviewBurst.score = score;
        reviewBurst.UpdateFlags();

        if(reviewBurst.isWrong)
        {
            reviewBurst.searchResult = bclf.ReviewSearch(pBurst);   
            reviewList.emplace_back(std::move(reviewBurst));
        }
    }

    strResult = agt.FetchResult();

    if( !reviewList.empty() && strResult == instance.front()->GetLabel())
    {
        reviewList.clear();
    }

    return reviewList;
}

ReviewBook BoClassifier::Predict(std::unordered_map<uint16_t, BurstGroups>* testset,  ResultBundle& result, bool reviewEnable)
{
    std::mutex reviewMutex;
    ReviewBook rBook;

    // Step 1: 数据预处理（不变）
    std::vector<BurstVec*> allBgroups;
    for (auto& [_, bgroups] : *testset) {
        for (auto& bgroup : bgroups) {
            allBgroups.push_back(&bgroup);
        }
    }

    // Step 2: 预分配结果容器
    std::vector<std::string> y_pred(allBgroups.size());
    std::vector<std::string> y_true(allBgroups.size());

    // Step 3: 并行处理实现
    const size_t num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;

    // 通用并行处理模板
    auto parallel_process = [&](auto&& func, auto&& output) {
        const size_t total = allBgroups.size();
        const size_t chunk_size = total / num_threads;
        const size_t remainder = total % num_threads;
    
        for (size_t t = 0; t < num_threads; ++t) {
            const size_t start = t * chunk_size + std::min(t, remainder);
            const size_t end = start + chunk_size + (t < remainder ? 1 : 0);
            if (start >= end) continue; // 避免空任务
    
            workers.emplace_back([=, &allBgroups, &output] {
                for (size_t i = start; i < end; ++i) {
                    output[i] = func(allBgroups[i]);
                }
            });
        }

        for (auto& worker : workers) worker.join();
        workers.clear();
    };
        
    // 并行预测
    parallel_process([&](BurstVec* bg) { 
        std::string result;
        
        std::vector<groundnut::ReviewBurst> reviewList = Predict(*bg, result, reviewEnable); 
        {
            std::unique_lock<std::mutex> lock(reviewMutex);
            rBook.reviewBook.push_back(reviewList);
        }
        return result;
    }, y_pred);

    // 并行获取标签
    parallel_process([](BurstVec* bg) { 
        return (*bg)[0]->GetLabel(); 
    }, y_true);

    result.Update(y_true, y_pred);
    return rBook;
}

}