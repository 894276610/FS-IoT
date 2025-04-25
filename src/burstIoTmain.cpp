#include "labSetting.h"
#include "experiment.h"

LabSetting GetDivisionSettings()
{
    LabSetting settings;

    settings.methodName = MethodEnum::FSIOT;
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; ;
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "division";
    //settings.config;
    settings.scenario = ExperimentEnum::BURST_INTRH_EVALUATION;
    settings.config.burstTrh.inTrh = {2,0};
    settings.start = 0.1; // sec slotduration
    settings.end = 0.5;
    settings.step = 0.1;
    settings.review = false;
    return settings;
}

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;

    settings.methodName = MethodEnum::FSIOT;
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; // "NEUKI2019"; //"UNSW201620"; //"NEUKI2019" //IOTBEHAV2021
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "hbd";
    settings.scenario = ExperimentEnum::FEW_SHOTS;
    settings.config.burstTrh.inTrh = {2,0};
    settings.start = 30; // sec slotduration
    settings.end = 30;
    settings.step = 30;
    settings.review = false;
    return settings;
}

int main() {
   
LabSetting settings = GetFewShotSettingTemplate();
std::unique_ptr<Experiment> pExpreiment = ExperimentFactory::CreateExperiment(settings);
pExpreiment->Run();

// Create an experiment
//   if(settings.experimentMode == "ipc")
//   {
//       InstancePercentLab(settings);
//   }
//   else if(settings.experimentMode == "hbd")
//   {
//       HourBudgetLab(settings);
//   }
//   else if(settings.experimentMode == "division")
//   {
//       DivisionLab(settings);
//   }
}