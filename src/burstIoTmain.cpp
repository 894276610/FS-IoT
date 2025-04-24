#include "labSetting.h"

void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
void DivisionLab(LabSetting settings);

LabSetting GetDivisionSettings()
{
    LabSetting settings;

    settings.methodName = "burstiot";
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = "UNSW201620";
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "division";
    //settings.config;
    settings.scenario = "inTrh";
    settings.config.burstTrh.inTrh = {2,0};
    settings.start = 0.1; // sec slotduration
    settings.end = 0.5;
    settings.step = 0.1;
    settings.review = false;
    return settings;
}

LabSetting GetHourBudgetSettings()
{
    LabSetting settings;

    settings.methodName = "burstiot";
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = "IOTBEHAV2021"; // "NEUKI2019"; //"UNSW201620"; //"NEUKI2019" //IOTBEHAV2021
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "hbd";
    settings.scenario = "hbd";
    settings.config.burstTrh.inTrh = {2,0};
    settings.start = 30; // sec slotduration
    settings.end = 30;
    settings.step = 30;
    settings.review = false;
    return settings;
}

int main() {
    
  LabSetting settings = GetHourBudgetSettings();
//   LabSetting settings = GetDivisionSettings();

  if(settings.experimentMode == "ipc")
  {
      InstancePercentLab(settings);
  }
  else if(settings.experimentMode == "hbd")
  {
      HourBudgetLab(settings);
  }
  else if(settings.experimentMode == "division")
  {
      DivisionLab(settings);
  }
}