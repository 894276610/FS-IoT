
#include "labSetting.h"
#include "experiment.h"
#include <range/v3/all.hpp>
#include <iostream>
#include <string>

void FewShotLabs() {

    std::vector<MethodEnum> methods = {MethodEnum::FSIOT, MethodEnum::BYTEIOT, MethodEnum::SHAHID};//MethodEnum::FSIOT, 
    std::vector<groundnut::DatasetEnum> datasets = { groundnut::DatasetEnum::UNSW201620, groundnut::DatasetEnum::BehavIoT2021};
    std::vector<ExperimentEnum> scenarios = {ExperimentEnum::FEW_SHOTS};

    LabSetting settings = GetFewShotSettingTemplate();
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.start = 30; // 30 minute
    settings.end = 300;  // 300 minute
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
    FewShotLabs();
}