#include "boClassifier.h"
#include <set>
#include <cmath>
#include "utils-metric.h"
#include <thread>

namespace groundnut{
   
void Aggregator::AddBurstPrediction(std::shared_ptr<KBurst> pBurst, BurstPrediction prediction)
{
    // predict names
    for(auto& pburst : prediction.nearTrainBursts)
    {
        this->devNameSet.insert(pburst->GetLabel());
    }

    // calculate score
    float score =  1.0 * log(pBurst->GetUniPktNum() + 1) / (devNameSet.size() * (sqrt(prediction.minDistance) + 1));

    // add to scoreBoard
    for(std::string name: devNameSet)
    {
        auto [it, inserted] = scoreBoard.try_emplace(name, score);
        
        if(!inserted)
        {
            it->second += score;
        }
    }
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

std::string BoClassifier::Predict(BurstVec instance)
{
    Aggregator agt;
    for(auto& pBurst: instance)
    {
        BurstPrediction prediction = bclf.Predict(pBurst);
        agt.AddBurstPrediction(pBurst, prediction);
    }

    return agt.FetchResult();
}

// multithreadings
ClassificationMetrics BoClassifier::Predict(std::unordered_map<uint16_t, BurstGroups>* testset)
{
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
          const size_t chunk_size = allBgroups.size() / num_threads;
          size_t remainder = allBgroups.size() % num_threads;
  
          for (size_t t = 0; t < num_threads; ++t) {
              const size_t start = t * chunk_size + std::min(t, remainder);
              const size_t end = (t + 1) * chunk_size + std::min(t + 1, remainder);
  
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
      parallel_process([this](BurstVec* bg) { 
          std::cout << "Thread ID: " << std::this_thread::get_id() << '\n';
          return Predict(*bg); 
      }, predictionVec);
  
      // 并行获取标签
      parallel_process([](BurstVec* bg) { 
          return (*bg)[0]->GetLabel(); 
      }, actualVec);
  
      return calculate_metrics(predictionVec, actualVec);
}

}