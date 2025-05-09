
#include "labSetting.h"
#include "experiment.h"
#include <range/v3/all.hpp>
#include <iostream>
#include <string>

void FewShotLabs(std::string baseFolder) {

    std::vector<MethodEnum> methods = {MethodEnum::FSIOT};  //, MethodEnum::AHMED}; // { MethodEnum::AHMED , MethodEnum::FSIOT, MethodEnum::BYTEIOT, MethodEnum::SHAHID};
    std::vector<groundnut::DatasetEnum> datasets = { groundnut::DatasetEnum::BehavIoT2021, groundnut::DatasetEnum::UNSW201620}; //groundnut::DatasetEnum::UNSW201620 }; //, groundnut::DatasetEnum::BehavIoT2021};//groundnut::DatasetEnum::UNSWMVP};
    std::vector<ExperimentEnum> scenarios = {ExperimentEnum::FEW_SHOTS};

    LabSetting settings = GetFewShotSettingTemplate();
    settings.baseFolder = baseFolder;
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.start = 30; // 30 minute
    settings.end = 60;  // 300 minute
    settings.step = 30;

    auto combinations = ranges::views::cartesian_product(methods, datasets, scenarios);
    for (const auto& [method, dataset, scenario] : combinations) {
        settings.methodName = method;
        settings.datasetName = dataset;
        settings.scenario = scenario;

        // std::unique_ptr<DivisionInterface> divExperiment = ExperimentFactory::CreateDivisionExperiment(settings);
        // divExperiment->RunDivision();

        std::unique_ptr<FewShotsInterface> experiment = ExperimentFactory::CreateFewShotsExperiment(settings);
        experiment->RunFewShot();
    }
}

void DivisionLabs(std::string baseFolder) {
    std::vector<MethodEnum> methods = {MethodEnum::FSIOT};  
    std::vector<groundnut::DatasetEnum> datasets = { groundnut::DatasetEnum::BehavIoT2021, groundnut::DatasetEnum::UNSW201620}; //groundnut::DatasetEnum::UNSW201620 }; //, groundnut::DatasetEnum::BehavIoT2021};//groundnut::DatasetEnum::UNSWMVP};
    std::vector<ExperimentEnum> scenarios = {ExperimentEnum::DIVISION};

    LabSetting settings = GetDivisionSettingTemplate();
    settings.baseFolder = baseFolder;
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.start = 30; // 30 minute
    settings.end = 60;  // 300 minute
    settings.step = 30;

    auto combinations = ranges::views::cartesian_product(methods, datasets, scenarios);

    for (const auto& [method, dataset, scenario] : combinations) {
        settings.methodName = method;
        settings.datasetName = dataset;
        settings.scenario = scenario;

        std::unique_ptr<DivisionInterface> divExperiment = ExperimentFactory::CreateDivisionExperiment(settings);
        divExperiment->RunDivision();
    }
}
int main(){
    std::string baseFolder = "/media/kunling/dpan/IoT/";
    py::initialize_interpreter();
    //FewShotLabs(baseFolder);
    DivisionLabs(baseFolder);   
    py::finalize_interpreter();
}