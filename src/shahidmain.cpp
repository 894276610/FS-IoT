#include "magic_enum.hpp"
#include "kpacket.h"
#include <iostream>

#include "burstClassifier.h"
#include "boost-serialize.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "boClassifier.h"
#include "kburst.h"
#include "utils-metric.h"
#include <fstream>
#include "timer.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"

#include "labSetting.h"

// void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
// void DivisionLab(LabSetting settings);

LabSetting GetHourBudgetSettings()
{
    LabSetting settings;
    settings.methodName = "shahid";
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = "UNSW201620";
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "hbd";
    settings.scenario = "hbd";
    // settings.config.burstTrh.inTrh = {2,0};
    settings.start = 30; // sec slotduration
    settings.end = 300;
    settings.step = 30;
    // settings.review = false;
    return settings;
}

int main() {
    
    LabSetting settings = GetHourBudgetSettings();

    if(settings.experimentMode == "ipc")
    {
        //InstancePercentLab(settings);
    }
    else if(settings.experimentMode == "hbd")
    {
        HourBudgetLab(settings);
    }
    else if(settings.experimentMode == "division")
    {
        // DivisionLab(settings);
    }
}