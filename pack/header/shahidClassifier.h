#ifndef _SHAHID_CLASSIFIER_H_
#define _SHAHID_CLASSIFIER_H_

#include "shahidDataset.h"
namespace groundnut
{

class ShahidClassifier
{

public:
ShahidClassifier(std::vector<groundnut::KDevice> devicesVec):devicesVec(devicesVec){}

void Train(std::unordered_map<uint16_t, ShahidSlots>* trainset);
std::string Predict(const ShahidStream stream);
std::string Predict(const ShahidSlot slot);

void Predict(std::unordered_map<uint16_t, ShahidSlots>* testset, std::vector<std::string>& y_true, std::vector<std::string>& y_pred);

private:
std::vector<groundnut::KDevice> devicesVec; 

mlpack::RandomForest<> rf;


};

}

#endif