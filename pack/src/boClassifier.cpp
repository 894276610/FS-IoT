#include "boClassifier.h"
#include <set>
#include <cmath>
#include "utils-metric.h"
#include <thread>

namespace groundnut{

void ReviewBurst::UpdateFlags()
{
    std::cout << "before update flags" << std::endl;
    if(prediction.nearTrainBursts.size() == 0)
    {
        isWrong = false;
        isFullyCorrect = false;
        std::cout << "after update flags" << std::endl;

        return;
    }


    if(prediction.nearTrainBursts.size() == 1)
    {
        isFullyCorrect = prediction.nearTrainBursts[0]->GetLabel() == testBurst->GetLabel();
        isWrong = !isFullyCorrect;
        std::cout << "after update flags" << std::endl;
        return;
    }

    isFullyCorrect = false;
    for(auto& nearBurst : prediction.nearTrainBursts)
    {      
        if(nearBurst->GetLabel() == testBurst->GetLabel())
        {
            isWrong = false;
            std::cout << "after update flags" << std::endl;

            return;
        }
    }

    isWrong = true;
    std::cout << "after update flags" << std::endl;

    return;        
}

float Aggregator::AddBurstPrediction(std::shared_ptr<KBurst> pBurst, SearchResult prediction, bool reviewEnable)
{
    ReviewBurst reviewBurst;

    this->devNameSet.clear();
    
    // predict names
    for(auto& pburst : prediction.nearTrainBursts)
    {
        this->devNameSet.insert(pburst->GetLabel());
    }

    // calculate score
    float score =  log(pBurst->GetUniPktNum() + 1) / (devNameSet.size() * (sqrt(prediction.minDistance) + 1));

    // add to scoreBoard
    for(std::string name: devNameSet)
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
    std::vector<std::pair<std::string, float>> vec(scoreBoard.begin(), scoreBoard.end());

    sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        if (a.second == b.second)
            return a.first < b.first; 
        return a.second > b.second;    
    });

    if(vec.size() > 0)
    {
        return vec[0].first;
    }
    else
    {
        return "unknown";
    }
}

void BoClassifier::Train(std::unordered_map<uint16_t, BurstGroups>* trainset)
{
    bclf.Train(trainset);
}

std::vector<ReviewBurst> BoClassifier::Predict(BurstVec instance, std::string& strResult, bool reviewEnable)
{
    Aggregator agt;
    std::vector<ReviewBurst> reviewList;

    //std::cout << "enter predict instance" <<std::endl;

    for(auto& pBurst: instance)
    {
        SearchResult prediction = bclf.Predict(pBurst);
        std::cout << "before add burst prediction" << std::endl;
        float score = agt.AddBurstPrediction(pBurst, prediction);

        if(!reviewEnable)
        {
            continue;
        }

        //std::cout << "review only part" <<std::endl;
        // review logic

        ReviewBurst reviewBurst;    
        reviewBurst.testBurst = pBurst;
        reviewBurst.prediction = prediction;
        reviewBurst.score = score;
        reviewBurst.UpdateFlags();
        //reviewBurst.searchResult = bclf.ReviewSearch(pBurst);   
        reviewList.push_back(reviewBurst);
    }

    strResult = agt.FetchResult();
    return reviewList;
}

// multithreadings
ReviewBook BoClassifier::Predict(std::unordered_map<uint16_t, BurstGroups>* testset, ClassificationMetrics& metric, bool reviewEnable)
{
    std::mutex reviewMutex;
    ReviewBook rBook;

    //std::cout << "start predict" <<std::endl;
    // Step 1: 数据预处理（不变）
    std::vector<BurstVec*> allBgroups;
    for (auto& [_, bgroups] : *testset) {
        for (auto& bgroup : bgroups) {
            allBgroups.push_back(&bgroup);
        }
    }

    // Step 2: 预分配结果容器
    std::vector<std::string> predictionVec(allBgroups.size());
    std::vector<std::string> actualVec(allBgroups.size());

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
    std::cout << "before parallel prediction" <<std::endl;

    parallel_process([&](BurstVec* bg) { 
        std::string result;
        //std::cout << "parallel before prediction of one instance" << std::endl;
        
        std::vector<groundnut::ReviewBurst> reviewList = Predict(*bg, result, reviewEnable); 
        {
            std::unique_lock<std::mutex> lock(reviewMutex);
            rBook.reviewBook.push_back(reviewList);
        }
        //std::cout << "parallel after prediction of one instance" << std::endl;
        return result;
    }, predictionVec);

    // 并行获取标签
    parallel_process([](BurstVec* bg) { 
        return (*bg)[0]->GetLabel(); 
    }, actualVec);

    std::cout << "actual vec size" << actualVec.size() << std::endl;
    std::cout << "prediction vec size" << predictionVec.size() << std::endl;

    metric = calculate_metrics(actualVec, predictionVec);
    return rBook;
}

}