#include "experiment.h"
#include "timer.h"
#include "boClassifier.h"

void FewShotsExperiment::Run()
{
    groundnut::DatasetEnum datasetName = this->setting.datasetName;
    int& budget = this->setting.config.trainBudget;
    
    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices(setting.GetDeviceMappingFilePath());
    pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());

    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        std::cout << "Training budget: " << budget << " minute." << std::endl;

        Instrumentor::Get().BeginSession("FewShotsExperiment", setting.GetTimeCostFilePath());
        PROFILE_SCOPE("TimeOverhead");
             
        groundnut::BurstDataset burstDataset(datasetName, setting.config);
        burstDataset.Load(pktDataset);
        burstDataset.TrainTestSplitByTime(budget);
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();
        
        groundnut::BoClassifier clf(setting.clfConfig);
        clf.Train(&trainset);
        std::cout << "Finish training." << std::endl;

        ResultBundle result;
        groundnut::ReviewBook rbook = clf.Predict(&testset, result, setting.review);
        std::cout << "Finish prediction." << std::endl;
        Instrumentor::Get().EndSession();
        
        result.SaveCsv(setting.GetResultCsvPath());
        rbook.Tofile(setting.GetReviewPath());
    }
}

std::unique_ptr<Experiment> CreateExperiment(LabSetting setting)
{
    switch (setting.scenario) {
        case ExperimentEnum::FEW_SHOTS:   
            return std::make_unique<FewShotsExperiment>(setting);
        // case ExperimentType::BURST_EXTRACTION return 
        // case ExperimentType::SAME_ACCURACY
        default: 
            throw std::invalid_argument("Unknown Experiment Type");
    }
}
