#ifndef _LABSETTING_H_
#define _LABSETTING_H_

#include <string>
#include <sstream>
#include "burstDataset.h"
#include "burstClassifier.h"

struct LabSetting
{
    std::string baseFolder = "/media/kunling/BigE/";
    std::string datasetName = "UNSW201620";
    std::string mappingFolder = "/home/kunling/BurstIoT/mappings/";
    std::string experimentMode = "ipc";
    groundnut::ConfigBurstDataset config;
    groundnut::ConfigBurstClf clfConfig;
    float start = 0.01;
    float end = 0.15;
    float step = 0.01;
    bool review = false;

    std::string ToString()
    {
        std::stringstream ss;
        ss << "-" << experimentMode << config.ToString() << clfConfig.ToString();
        return ss.str();
    }
};

#endif