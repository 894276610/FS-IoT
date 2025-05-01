
#include "labSetting.h"
#include "experiment.h"
#include <range/v3/all.hpp>
#include <iostream>
#include <string>

void FewShotLabs() {

    std::vector<MethodEnum> methods = { MethodEnum::AHMED , MethodEnum::FSIOT, MethodEnum::BYTEIOT, MethodEnum::SHAHID};
    std::vector<groundnut::DatasetEnum> datasets = { groundnut::DatasetEnum::UNSW201620, groundnut::DatasetEnum::BehavIoT2021};//groundnut::DatasetEnum::UNSWMVP};
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
        std::cout << "For a new experiment" << std::endl;
        std::unique_ptr<Experiment> experiment = ExperimentFactory::CreateExperiment(settings);
        experiment->Run();
    }
}

void PostProcessing()
{
    // 1. Get the summary csv files

    // 2. Generate Confusion Matrix

    // 3. Draw line chart for Accuracy

}

int main(){
    py::initialize_interpreter();
    FewShotLabs();
    py::finalize_interpreter();
}