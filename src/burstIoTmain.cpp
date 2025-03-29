#include "labSetting.h"

void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);

int main() {
  LabSetting settings;

  settings.baseFolder = "/media/kunling/BigE/";
  settings.datasetName = "UNSW201620";
  settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
  settings.experimentMode = "hbd";
  settings.config;
  settings.start = 120;
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
}