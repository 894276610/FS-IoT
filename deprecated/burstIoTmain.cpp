#include "labSetting.h"
#include "experiment.h"

LabSetting GetDivisionSettingTemplate()
{
    LabSetting settings;
    settings.baseFolder = "/media/kunling/BigE/";

    settings.methodName = MethodEnum::FSIOT;
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; 
    settings.scenario = ExperimentEnum::DIVISION;

    settings.independentArg = IndependentArgEnum::IN_TRH;
    //settings.config;
    settings.burstTrh.inTrh = {2,0};
    settings.start = 0.1; // sec slotduration
    settings.end = 0.5;
    settings.step = 0.1;
    settings.clfConfig.review = false;
    return settings;
}

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;
    settings.baseFolder = "/media/kunling/BigE/";

    settings.methodName = MethodEnum::FSIOT;
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; // "NEUKI2019"; //"UNSW201620"; //"NEUKI2019" //IOTBEHAV2021
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.scenario = ExperimentEnum::FEW_SHOTS;
    settings.burstTrh.inTrh = {2,0};
    settings.start = 30; // sec slotduration
    settings.end = 30;
    settings.step = 30;
    settings.clfConfig.review = false;
    return settings;
}

int main() {
   
LabSetting settings = GetFewShotSettingTemplate();
ExperimentFactory::CreateExperiment(settings)->Run();

}