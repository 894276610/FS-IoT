#include "labSetting.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"
#include "kburst.h"

std::string GetMetricPath(LabSetting settings);
std::string GetPredCsvPath(LabSetting settings);
std::string NameDivMetric(LabSetting settings, std::string mode);

void InstancePercentLab(LabSetting settings)
{   
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string outFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetOutName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    pktDataset.AutoLoad(outFolder,pktDatasetOutName);

    float& trainRate =  settings.config.trainRate;
    for(trainRate = settings.start; trainRate <= settings.end; trainRate+= settings.step)
    {
        ClassificationMetrics metric;
        groundnut::ByteIoTClassifier clf;
        groundnut::ByteIoTDataset byteIoTDataset(datasetName, settings.config);

        byteIoTDataset.Load(pktDataset);
        byteIoTDataset.TrainTestSplit();
        auto& trainset = byteIoTDataset.GetTrainset();
        auto& testset = byteIoTDataset.GetTestset();
        std::vector<std::string> y_true, y_pred;
    
        clf.Train(&trainset); 
        clf.Predict(&testset, metric, y_true, y_pred, false);

        SerializePrediction(y_true, y_pred, GetPredCsvPath(settings));
        std::ofstream outMetric(GetMetricPath(settings));
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
    pktDataset.AutoLoad(outFolder,pktDatasetOutName);

    int& budget = settings.config.trainBudget;
    for( budget = settings.start; budget <= settings.end; budget+= settings.step)
    {
        std::cout << "budget: " << budget << "min" << std::endl;

        ClassificationMetrics metric;
        groundnut::ByteIoTClassifier clf;
        groundnut::ByteIoTDataset byteIoTDataset(datasetName, settings.config);

        byteIoTDataset.Load(pktDataset);
        byteIoTDataset.TrainTestSplitByTime(budget);
        auto& trainset = byteIoTDataset.GetTrainset();
        auto& testset  = byteIoTDataset.GetTestset();
        std::vector<std::string> y_true, y_pred;
    
        clf.Train(&trainset);

        std::cout << "after training." << std::endl;
        std::cout << "start prediction." << std::endl;
        clf.Predict(&testset, metric, y_true, y_pred, settings.review);
        std::cout << "finish prediction." << std::endl;
        
        SerializePrediction(y_true, y_pred, GetPredCsvPath(settings));
        std::ofstream outMetric(GetMetricPath(settings));
        outMetric << ToString(metric);
        outMetric.close();
    }
}

// @input:  change duration variable
// @output: dataset partition metric.

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

    int& slotDuration =  settings.config.slotDuration;
    for(slotDuration = settings.start; slotDuration <= settings.end; slotDuration+= settings.step)
    {
        
        groundnut::ByteIoTDataset byteIoTDataset(datasetName, settings.config);
        std::ofstream outMetric(NameDivMetric(settings, "fixed"));
        groundnut::BurstVec totalSamples;

        byteIoTDataset.Load(pktDataset);
        auto& rawMap = byteIoTDataset.GetRawMap();

        for(auto& [devid, burstVec]: rawMap)
        {
            std::string deviceName = byteIoTDataset.GetDevicesVec()[devid].GetLabel();
            groundnut::DivMetric metric = byteIoTDataset.GenDivMetric(deviceName, burstVec);
            totalSamples.insert(totalSamples.end(), burstVec.begin(), burstVec.end());
            outMetric << metric.ToString();
        }

        groundnut::DivMetric metric = byteIoTDataset.GenDivMetric("total", totalSamples);
        outMetric << metric.ToString();        
        outMetric.close();
    }
}