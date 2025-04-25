#include "experiment.h"
#include "timer.h"
#include "boClassifier.h"
#include "byteIoTDataset.h"
#include "byteIoTClassifier.h"
#include "shahidDataset.h"
#include "shahidClassifier.h"

void ShahidFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.config.trainBudget;

    Instrumentor::Get().BeginSession("TimeOverhead", setting.GetTimeCostFilePath());
    std::cout << "budget: " << budget << "min" << std::endl;

    groundnut::ShahidDataset shahidDataset(setting.datasetName, 6, setting.config.slotDuration, 600);
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
    int budget = setting.config.trainBudget;

    std::cout << "budget: " << budget << "min" << std::endl;
    Instrumentor::Get().BeginSession("TimeOverhead", setting.GetTimeCostFilePath());
    
    groundnut::ByteIoTClassifier clf;
    groundnut::ByteIoTDataset byteIoTDataset( setting.datasetName, setting.config);

    byteIoTDataset.Load(pktDataset);
    byteIoTDataset.TrainTestSplitByTime(budget);
    auto& trainset = byteIoTDataset.GetTrainset();
    auto& testset  = byteIoTDataset.GetTestset();
    std::vector<std::string> y_true, y_pred;

    ResultBundle result;

    clf.Train(&trainset);
    clf.Predict(&testset, result, setting.review);
    Instrumentor::Get().EndSession();
    
    result.SaveCsv(setting.GetResultCsvPath());
}

void FSIoTFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.config.trainBudget;
    std::cout << "Training budget: " << budget << " minute." << std::endl;

    Instrumentor::Get().BeginSession("FewShotsExperiment", setting.GetTimeCostFilePath());
    PROFILE_SCOPE("TimeOverhead");

    groundnut::BurstDataset burstDataset(pktDataset.GetName(), setting.config);
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

void FewShotsExperiment::Run()
{
    groundnut::DatasetEnum datasetName = this->setting.datasetName;
    int& budget = this->setting.config.trainBudget;
    
    pktDataset.AddTragetDevices(setting.GetDeviceMappingFilePath());
    pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());

    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        RunOnce(setting);
        
    }
}
