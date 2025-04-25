#include "experiment.h"
#include "timer.h"
#include "boClassifier.h"
#include "byteIoTDataset.h"
#include "byteIoTClassifier.h"
#include "shahidDataset.h"
#include "shahidClassifier.h"

void ShahidFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.trainBudget;

    Instrumentor::Get().BeginSession("TimeOverhead", setting.GetTimeCostFilePath());
    std::cout << "budget: " << budget << "min" << std::endl;

    groundnut::ShahidDataset shahidDataset(setting.datasetName, 6, setting.slotDuration, 600);
    std::unordered_map<uint16_t, groundnut::ShahidSlots> trainSet, testSet;

    shahidDataset.Load(pktDataset);

    shahidDataset.TrainTestSplit(trainSet, testSet, budget);

    groundnut::ShahidClassifier clf(shahidDataset.devicesVec);
    ResultBundle result;
    clf.Train(&trainSet);
    clf.Predict(&testSet, result);
    Instrumentor::Get().EndSession();

    result.SaveCsv(setting.GetResultCsvPath());
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
    Instrumentor::Get().BeginSession("TimeOverhead", setting.GetTimeCostFilePath());
 
    groundnut::ByteIoTDataset byteIoTDataset( setting.datasetName, config);
    byteIoTDataset.Load(pktDataset);
    byteIoTDataset.TrainTestSplitByTime(budget);
    auto& trainset = byteIoTDataset.GetTrainset();
    auto& testset  = byteIoTDataset.GetTestset();

    clf.Train(&trainset);
    clf.Predict(&testset, result, setting.clfConfig.review);
    Instrumentor::Get().EndSession();
    
    result.SaveCsv(setting.GetResultCsvPath());
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

    Instrumentor::Get().BeginSession("FewShotsExperiment", setting.GetTimeCostFilePath());
    PROFILE_SCOPE("TimeOverhead");

    groundnut::BurstDataset burstDataset(pktDataset.GetName(), config);
    burstDataset.Load(pktDataset);
    burstDataset.TrainTestSplitByTime(budget);
    auto& trainset = burstDataset.GetTrainset();
    auto& testset = burstDataset.GetTestset();
    
    groundnut::BoClassifier clf(setting.clfConfig);
    clf.Train(&trainset);
    std::cout << "Finish training." << std::endl;

    ResultBundle result;
    groundnut::ReviewBook rbook = clf.Predict(&testset, result, setting.clfConfig.review);
    std::cout << "Finish prediction." << std::endl;
    Instrumentor::Get().EndSession();
    
    result.SaveCsv(setting.GetResultCsvPath());
    rbook.Tofile(setting.GetReviewPath());
}

void FewShotsExperiment::Run()
{
    groundnut::DatasetEnum datasetName = this->setting.datasetName;
    int& budget = this->setting.trainBudget;
    
    pktDataset.AddTragetDevices(setting.GetDeviceMappingFilePath());
    pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());

    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        RunOnce(setting);
    }
}
