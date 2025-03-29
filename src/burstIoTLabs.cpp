#include "labSetting.h"

#include "boClassifier.h"
#include "burstDataset.h"

std::string NameClassifyMetric(LabSetting settings, std::string methodName);

void InstancePercentLab(LabSetting settings)
{ 
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string datasetRootFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetSerialName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    pktDataset.AutoLoad(datasetRootFolder, pktDatasetSerialName);
    
    float& trainRate = settings.config.trainRate;
    settings.config.trainBudget = 100000000;
    for(trainRate = settings.start; trainRate <= settings.end; trainRate += settings.step)
    {
        std::cout << "trainRate: " << trainRate << std::endl;

        groundnut::BurstDataset burstDataset(datasetName, settings.config);
        ClassificationMetrics metric;

        burstDataset.Load(pktDataset);
        burstDataset.TrainTestSplit();
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();

        groundnut::BoClassifier boclf(settings.clfConfig);
        boclf.Train(&trainset);
        groundnut::ReviewBook reviewBook = boclf.Predict(&testset, metric, settings.review);

        std::ofstream outMetric(NameClassifyMetric(settings,"burstiot"));
        outMetric << ToString(metric);
        outMetric.close(); 
    }
}

void HourBudgetLab(LabSetting settings)
{
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string datasetRootFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetSerialName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    pktDataset.AutoLoad(datasetRootFolder, pktDatasetSerialName);

    int& budget = settings.config.trainBudget;
    for(budget = settings.start; budget <= settings.end; budget+= settings.step)
    {
        std::cout << "budget: " << budget << " minute" << std::endl;

        ClassificationMetrics metric;
        groundnut::BurstDataset burstDataset(datasetName, settings.config);

        burstDataset.Load(pktDataset);
        burstDataset.TrainTestSplitByTime(budget);
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();
    
        groundnut::BoClassifier clf(settings.clfConfig);
        clf.Train(&trainset);
        clf.Predict(&testset, metric, settings.review);
        
        std::ofstream outMetric(NameClassifyMetric(settings,"burstiot"));
        outMetric << ToString(metric);
        outMetric.close();
    }
}