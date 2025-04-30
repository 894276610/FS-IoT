#ifndef _SHAHID_CLASSIFIER_H_
#define _SHAHID_CLASSIFIER_H_

#include "shahidDataset.h"
#include "resultBundle.h"
namespace groundnut
{

class ShahidClassifier
{

public:
ShahidClassifier( std::unordered_map<std::string, groundnut::KDevice> devicesMap)
{
    devicesVec.clear();
    devicesVec.resize(devicesMap.size());
    for(auto [mac_string, kdevice] : devicesMap)
    {
        devicesVec[kdevice.GetDevId()] = kdevice;
    }
}

void Train(std::unordered_map<uint16_t, ShahidSlots>* trainset);
std::string Predict(const ShahidStream stream);
std::string Predict(const ShahidSlot slot);

void Predict(std::unordered_map<uint16_t, ShahidSlots>* testset, ResultBundle& result);

private:
std::vector<groundnut::KDevice> devicesVec; 
mlpack::RandomForest<> rf;

};

}

#endif