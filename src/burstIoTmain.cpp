#include "labSetting.h"

void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
void DivisionLab(LabSetting settings);

int main() {
  LabSetting settings;
  // division: change ouTrh
 
  settings.baseFolder = "/media/kunling/BigE/";
  settings.datasetName = "UNSW201620";
  settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
  settings.experimentMode = "hbd";
  settings.config.burstTrh.ouTrh = {60,0};
  settings.config.burstTrh.longShortEnable = false;
  settings.start = 30; 
  settings.end = 600;
  settings.step = 30;
  settings.review = false;
  
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