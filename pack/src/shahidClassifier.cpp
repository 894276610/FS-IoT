#include "shahidClassifier.h"
#include <mlpack.hpp>
#include "timer.h"

namespace groundnut
{

void ShahidClassifier::Train(std::unordered_map<uint16_t, ShahidSlots>* trainset)
{
    PROFILE_SCOPE("Train");
    arma::mat trainMat;
    arma::Row<size_t> trainLabels;

    for(const auto& [deviceId, slots]: *trainset)
    {
        for(const auto& slot : slots)
        {
            for(const auto& stream : slot.streams)
            {
                trainMat.insert_cols(trainMat.n_cols, stream->features);
                trainLabels.insert_cols(trainLabels.n_cols, 1);
                trainLabels.back() = deviceId;
            }
        }
    }        

    // trainMat.print("trainMatrix!");
    // trainLabels.print("Labels");
    rf = mlpack::RandomForest<>(trainMat, trainLabels, trainset->size(), 100); // TODO I donnot know the parameter.
}

std::string ShahidClassifier::Predict(const ShahidStream stream)
{
    arma::Row<size_t> output;
    rf.Classify(stream.features, output);
    
    // stream.features.print("feature");
    // output.print("output");
    return devicesVec[output.at(0)].GetLabel();
}

std::string ShahidClassifier::Predict(const ShahidSlot slot)
{
    std::unordered_map<std::string, int> deviceCounts;
    
    // 统计1月每天的颜色
    for (const auto& stream: slot.streams) {
        std::string predictLabel = Predict(*stream);
        deviceCounts[predictLabel]++;
    }
    
    // 使用算法库找到最大值
    auto it = std::max_element(deviceCounts.begin(), deviceCounts.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
    
    return it->first;
}

void ShahidClassifier::Predict(std::unordered_map<uint16_t, ShahidSlots>* testset, ResultBundle& result)
{
    PROFILE_SCOPE("Identification");
    for(const auto& [devId, slots] : *testset)
    {
        for(const auto& slot : slots)
        {
            result.Add(Predict(slot), devicesVec[devId].GetLabel());
        }
    }
}

}