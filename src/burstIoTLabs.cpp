#include "labSetting.h"

#include "boClassifier.h"
#include "burstDataset.h"

void InstancePercentLab(LabSetting settings)
{ 
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string outFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetOutName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");

    if(std::filesystem::exists(outFolder + pktDatasetOutName))
    {
        pktDataset.LoadBin(outFolder + pktDatasetOutName);
    }
    else
    {
        pktDataset.LoadPcap(outFolder + "RAW");
        pktDataset.Serialize(outFolder+ pktDatasetOutName);  
    }
    
    for(float trainRate = settings.start; trainRate <= settings.end; trainRate += settings.step)
    {
        groundnut::BurstDataset burstDataset(datasetName, settings.config);
        ClassificationMetrics metric;

        burstDataset.Load(pktDataset);
        burstDataset.TrainTestSplit();
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();

        groundnut::BoClassifier boclf(settings.clfConfig);
        boclf.Train(&trainset);
        groundnut::ReviewBook reviewBook = boclf.Predict(&testset, metric, false);

        std::ofstream outMetric(outFolder + settings.clfConfig.ToString() \
        + settings.config.ToString() + "metrics.txt");
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

    for(int budget = settings.start; budget <= settings.end; budget+= settings.step)
    {
        std::cout << "budget: " << budget << "x15min" << std::endl;

        ClassificationMetrics metric;

        groundnut::BurstDataset burstDataset(datasetName, settings.config);

        burstDataset.Load(pktDataset);
        float avgDeviceInstance = burstDataset.TrainTestSplitByTime(budget);
        auto& trainset = burstDataset.GetTrainset();
        auto& testset = burstDataset.GetTestset();
    
        groundnut::BoClassifier clf(settings.clfConfig);
        clf.Train(&trainset);
        clf.Predict(&testset, metric, settings.review);
        
        std::string metricFileName;
        metricFileName += datasetRootFolder;
        metricFileName += settings.experimentMode;
        metricFileName += settings.config.ToString();
        metricFileName += "burstiot-metrics.txt";

        std::ofstream outMetric(metricFileName);
        float avgHr = avgDeviceInstance * settings.config.slotDuration/3600;
        outMetric << "Average Device Time: " << std::to_string(avgHr) << " hour" << std::endl;
        outMetric << ToString(metric);
        outMetric.close();
    }
}