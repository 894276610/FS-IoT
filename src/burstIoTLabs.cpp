#include "labSetting.h"

#include "boClassifier.h"
#include "burstDataset.h"
#include "time-utils.h"

std::string NameClassifyMetric(LabSetting settings, std::string methodName);
std::string NameDivMetric(LabSetting settings, std::string mode);
timespec Float2time(float ftime);

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

        std::cout << "before get train and test set" << std::endl;
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();
        
        std::cout << "before clf" << std::endl;
        groundnut::BoClassifier clf(settings.clfConfig);
        std::cout << "before train" << std::endl;
        clf.Train(&trainset);
        clf.Predict(&testset, metric, settings.review);
        
        std::ofstream outMetric(NameClassifyMetric(settings,"burstiot"));
        outMetric << ToString(metric);
        outMetric.close();
    }
}

void DivisionLab(LabSetting settings)
{
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string outFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetOutName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    pktDataset.AutoLoad(outFolder,pktDatasetOutName);

    timespec& ouTrh =  settings.config.burstTrh.ouTrh;
    for(ouTrh = Float2time(settings.start); ouTrh < Float2time(settings.end); ouTrh += settings.step)
    {
        groundnut::BurstDataset burstDataset(datasetName, settings.config);
        std::ofstream outMetric(NameDivMetric(settings, "burst"));
        groundnut::BurstGroups totalSamples;

        burstDataset.Load(pktDataset);
        auto& rawMap = burstDataset.GetRawMap();

        for(auto& [devid, burstGroups]: rawMap)
        {
            std::string deviceName = burstDataset.GetDevicesVec()[devid].GetLabel();
            groundnut::DivMetric metric = burstDataset.GenDivMetric(deviceName, burstGroups);
            totalSamples.insert(totalSamples.end(), burstGroups.begin(), burstGroups.end());
            outMetric << metric.ToString();
        }

        groundnut::DivMetric metric = burstDataset.GenDivMetric("total", totalSamples);
        outMetric << metric.ToString();        
        outMetric.close();
    }
}