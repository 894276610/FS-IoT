#include "byteIoTClassifier.h"
#include <future>
#include "boClassifier.h"
#include "timer.h"

namespace groundnut
{

void ByteIoTClassifier::Train(std::unordered_map<uint16_t, BurstVec>* trainset)
{
    this->oriTrainset = trainset;
}

std::string ByteIoTClassifier::Predict(const std::shared_ptr<KBurst> preInstnace)
{
    SearchResult result;    
    float min_distance = 100, distance;

    for(auto& [deviceId, instances] : *oriTrainset)
    {
        for(auto& instance : instances)
        {
            distance = preInstnace->Distance(instance);
            if (distance < min_distance)
            {
                result.nearTrainBursts.clear();
                result.nearTrainBursts.push_back(instance);
                result.minDistance = distance;
                min_distance = distance;
            }
            else if (distance == min_distance)
            {
                result.nearTrainBursts.push_back(instance);
            }
        }
    }
    return result.nearTrainBursts.front()->GetLabel();
}

void ByteIoTClassifier::Predict(std::unordered_map<uint16_t, BurstVec>* testset, ClassificationMetrics& metric, bool reviewEnable)
{
    PROFILE_SCOPE("predict");
    std::mutex reviewMutex;

    // Step 1: 数据预处理（不变）
    BurstVec allInstances;
    for (auto& [_, instanceVec] : *testset) { 
        allInstances.insert(allInstances.end(), instanceVec.begin(), instanceVec.end());
    }

    // Step 2: 预分配结果容器
    std::vector<std::string> predictionVec(allInstances.size());
    std::vector<std::string> actualVec(allInstances.size());

    // Step 3: 并行处理实现
    const size_t num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;

    // 通用并行处理模板
    auto parallel_process = [&](auto&& func, auto&& output) {
        const size_t total = allInstances.size();
        const size_t chunk_size = total / num_threads;
        const size_t remainder = total % num_threads;
    
        for (size_t t = 0; t < num_threads; ++t) {
            const size_t start = t * chunk_size + std::min(t, remainder);
            const size_t end = start + chunk_size + (t < remainder ? 1 : 0);
            if (start >= end) continue; // 避免空任务
    
            workers.emplace_back([=, &allInstances, &output] {
                for (size_t i = start; i < end; ++i) {
                    output[i] = func(allInstances[i]);
                }
            });
        }

        for (auto& worker : workers) worker.join();
        workers.clear();
    };
        
    // 并行预测
    parallel_process([&](std::shared_ptr<KBurst> instance) {      
        return Predict(instance); 
    }, predictionVec);

    // 并行获取标签
    parallel_process([](std::shared_ptr<KBurst> instance) { 
        return instance->GetLabel(); 
    }, actualVec);

    std::cout << "actual vec size" << actualVec.size() << std::endl;

    metric = calculate_metrics(actualVec, predictionVec);
}

}
