#include "experiment.h"
#include "timer.h"
#include "boClassifier.h"
#include "byteIoTDataset.h"
#include "byteIoTClassifier.h"
#include "shahidDataset.h"
#include "shahidClassifier.h"
#include "magic_enum.hpp"

void ShahidFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.trainBudget;

    Instrumentor::Get().BeginCsvSession("TimeOverhead", setting.GetTimeOverheadPath());
    std::cout << "budget: " << budget << "min" << std::endl;

    groundnut::ShahidDataset shahidDataset(pktDataset.GetName(), 6, setting.slotDuration, 600);
    std::unordered_map<uint16_t, groundnut::ShahidSlots> trainSet, testSet;

    shahidDataset.Load(pktDataset);
    shahidDataset.TrainTestSplit(trainSet, testSet, budget);

    groundnut::ShahidClassifier clf(shahidDataset.devicesVec);
    ResultBundle result;
    clf.Train(&trainSet);
    clf.Predict(&testSet, result);
    Instrumentor::Get().EndCsvSession();

    result.SaveCsv(setting.GetPredictionCsvPath());
}
void ByteIoTFewShotsExperiment::RunOnce(LabSetting setting)
{      
    int budget = setting.trainBudget;
    groundnut::ByteIoTClassifier clf;
    groundnut::ConfigDataset config;
    ResultBundle result;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;

    std::cout << "budget: " << budget << "min" << std::endl;
    Instrumentor::Get().BeginCsvSession("ByteIoT-FewShotsExperiment", setting.GetTimeOverheadPath());
 
    groundnut::ByteIoTDataset byteIoTDataset(pktDataset.GetName(), config);
    byteIoTDataset.Load(pktDataset);
    byteIoTDataset.TrainTestSplitByTime(budget);
    auto& trainset = byteIoTDataset.GetTrainset();
    auto& testset  = byteIoTDataset.GetTestset();

    clf.Train(&trainset);
    clf.Predict(&testset, result, setting.clfConfig.review);
    Instrumentor::Get().EndCsvSession();
    
    result.SaveCsv(setting.GetPredictionCsvPath());
}

void FSIoTFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.trainBudget;
    std::cout << "Training budget: " << budget << " minute." << std::endl;
    groundnut::ConfigBurstDataset config;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;
    config.burstTrh = setting.burstTrh;

    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());

    groundnut::BurstDataset burstDataset(pktDataset.GetName(), config);
    groundnut::BoClassifier clf(setting.clfConfig);
    ResultBundle result;
    groundnut::ReviewBook rbook;

    {
        PROFILE_SCOPE("Load", setting.ToString().c_str());
        burstDataset.Load(pktDataset);
    }

    {
        PROFILE_SCOPE("Split", setting.ToString().c_str());
        burstDataset.TrainTestSplitByTime(budget);
    }

    auto& trainset = burstDataset.GetTrainset();
    auto& testset = burstDataset.GetTestset();

    {
        PROFILE_SCOPE("Train", setting.ToString().c_str());
        clf.Train(&trainset);    
    }
   
    {
        PROFILE_SCOPE("Predict", setting.ToString().c_str());
        rbook = clf.Predict(&testset, result, setting.clfConfig.review);
    }

    Instrumentor::Get().EndCsvSession();
    
    result.SaveCsv(setting.GetPredictionCsvPath());

    if(!setting.clfConfig.review)
    {
        return;
    }

    rbook.Tofile(setting.GetReviewPath());
}

void FewShotsExperiment::Run()
{
    int& budget = this->setting.trainBudget;
    
    std::cout << "device count:" << pktDataset.GetDevicesMap().size() << std::endl;
    
    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());
    pktDataset.UpdateTargetDevices(setting.GetDeviceMappingFilePath());
    pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());
    Instrumentor::Get().EndCsvSession();

    std::cout << "device count:" << pktDataset.GetDevicesMap().size() << std::endl;
    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        RunOnce(setting);
    }

}
