#include "labSetting.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"

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

    for(float trainRate = settings.start; trainRate <= settings.end; trainRate+= settings.step)
    {
        settings.config.trainRate = trainRate;
        ClassificationMetrics metric;

        groundnut::ByteIoTDataset byteIoTDataset(datasetName, settings.config);

        byteIoTDataset.Load(pktDataset);
        float avgDeviceInstance = byteIoTDataset.TrainTestSplit();
        auto& trainset = byteIoTDataset.GetTrainset();
        auto& testset = byteIoTDataset.GetTestset();
    
        groundnut::ByteIoTClassifier clf;
        clf.Train(&trainset); 
        clf.Predict(&testset, metric, false);
        
        std::string metricFileName;
        metricFileName += outFolder;
        metricFileName += settings.experimentMode;
        metricFileName += settings.config.ToString();
        metricFileName += "byteiot-metrics.txt";

        std::ofstream outMetric(metricFileName);
        float avgHr = avgDeviceInstance * settings.config.slotDuration/3600;
        outMetric << "Average Device Time: " << std::to_string(avgHr) << " hour" << std::endl;
        outMetric << ToString(metric);
        outMetric.close();
    }
}

void HourBudgetLab(LabSetting settings)
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

    for(int budget = settings.start; budget <= settings.end; budget+= settings.step)
    {
        std::cout << "budget: " << budget << "x15min" << std::endl;

        ClassificationMetrics metric;

        groundnut::ByteIoTDataset byteIoTDataset(datasetName, settings.config);

        byteIoTDataset.Load(pktDataset);
        float avgDeviceInstance = byteIoTDataset.TrainTestSplitByTime(budget);
        auto& trainset = byteIoTDataset.GetTrainset();
        auto& testset = byteIoTDataset.GetTestset();
    
        groundnut::ByteIoTClassifier clf;
        clf.Train(&trainset);
        clf.Predict(&testset, metric, settings.review);
        
        std::string metricFileName;
        metricFileName += outFolder;
        metricFileName += settings.experimentMode;
        metricFileName += settings.config.ToString();
        metricFileName += "byteiot-metrics.txt";

        std::ofstream outMetric(metricFileName);
        float avgHr = avgDeviceInstance * settings.config.slotDuration/3600;
        outMetric << "Average Device Time: " << std::to_string(avgHr) << " hour" << std::endl;
        outMetric << ToString(metric);
        outMetric.close();
    }
}