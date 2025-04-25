#include "magic_enum.hpp"
#include "kpacket.h"
#include <iostream>

#include "burstClassifier.h"
#include "boost-serialize.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "boClassifier.h"
#include "kburst.h"
#include <fstream>
#include "timer.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"

#include "labSetting.h"

// void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
// void DivisionLab(LabSetting settings);

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;
    settings.methodName = MethodEnum::SHAHID;
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; // "NEUKI2019"; //"UNSW201620";
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "hbd";
    settings.scenario = ExperimentEnum::FEW_SHOTS;
    settings.config.trainRate = 0.5f;
    // settings.config.burstTrh.inTrh = {2,0};
    settings.start = 12300; // sec slotduration
    settings.end = 15000;
    settings.step = 90;
    // settings.review = false;
    return settings;
}

int main() {
    
    LabSetting settings = GetFewShotSettingTemplate();

    // if(settings.experimentMode == "ipc")
    // {
    //     //InstancePercentLab(settings);
    // }
    // else if(settings.experimentMode == "hbd")
    // {
    //     HourBudgetLab(settings);
    // }
    // else if(settings.experimentMode == "division")
    // {
    //     // DivisionLab(settings);
    // }
}