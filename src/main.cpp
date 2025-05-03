
#include "labSetting.h"
#include "experiment.h"
#include <range/v3/all.hpp>
#include <iostream>
#include <string>

void FewShotLabs() {

    std::vector<MethodEnum> methods = {MethodEnum::FSIOT, MethodEnum::AHMED}; // { MethodEnum::AHMED , MethodEnum::FSIOT, MethodEnum::BYTEIOT, MethodEnum::SHAHID};
    std::vector<groundnut::DatasetEnum> datasets = { groundnut::DatasetEnum::BehavIoT2021 }; //groundnut::DatasetEnum::UNSW201620 }; //, groundnut::DatasetEnum::BehavIoT2021};//groundnut::DatasetEnum::UNSWMVP};
    std::vector<ExperimentEnum> scenarios = {ExperimentEnum::FEW_SHOTS};

    LabSetting settings = GetFewShotSettingTemplate();
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.start = 30; // 30 minute
    settings.end = 30;  // 300 minute
    settings.step = 30;

    auto combinations = ranges::views::cartesian_product(methods, datasets, scenarios);
    for (const auto& [method, dataset, scenario] : combinations) {
        settings.methodName = method;
        settings.datasetName = dataset;
        settings.scenario = scenario;
        std::unique_ptr<Experiment> experiment = ExperimentFactory::CreateExperiment(settings);
        experiment->Run();
    }
}

int main(){
    py::initialize_interpreter();
    FewShotLabs();
    py::finalize_interpreter();
}