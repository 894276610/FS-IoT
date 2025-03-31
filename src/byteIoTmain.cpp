#include "magic_enum.hpp"
#include "kpacket.h"
#include <iostream>

#include "burstClassifier.h"
#include "boost-serialize.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "boClassifier.h"
#include "kburst.h"
#include "utils-metric.h"
#include <fstream>
#include "timer.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"

#include "labSetting.h"

void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
void DivisionLab(LabSetting settings);

int main() {
    LabSetting settings;

    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = "UNSW201620";
    settings.mappingFolder = "/home/kunling/BurstIoT/mappings/";
    settings.experimentMode = "hbd";
    //settings.config;
    settings.config.slotDuration = 15;

    settings.start = 30; // sec slotduration
    settings.end = 300;
    settings.step = 15;
    settings.review = false;

    if(settings.experimentMode == "ipc")
    {
        InstancePercentLab(settings);
    }
    else if(settings.experimentMode == "hbd")
    {
        HourBudgetLab(settings);
    }
    else if(settings.experimentMode == "division")
    {
        DivisionLab(settings);
    }
}

/*

    // init packet dataset and add target devices
    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    std::cout << "after adding devices"<<  std::endl;
    // load existing packet dataset or load from raw packets.
    if(std::filesystem::exists(outFolder + pktDatasetOutName))
    {
        std::cout << "load bin" << std::endl;
        pktDataset.LoadBin(outFolder + pktDatasetOutName);
        std::cout << "after load bin" << std::endl;
    }
    else
    {
        std::cout << "load raw"<< std::endl;
        pktDataset.LoadPcap(outFolder + "RAW");
        pktDataset.Serialize(outFolder+ pktDatasetOutName);  
        std::cout << "after load raw"<< std::endl;
    }
    
    std::cout << "after load pcap"<< std::endl;

    // init burst dataset
    groundnut::ConfigBurstDataset config;

    for(float rate = 0.01; rate <=0.02; rate+=0.01)
    {
        config.trainRate = rate;
        groundnut::ByteIoTDataset byteIoTDataset(datasetName, config);

        byteIoTDataset.Load(pktDataset);
        float avgDeviceInstance = byteIoTDataset.TrainTestSplit();
        auto& trainset = byteIoTDataset.GetTrainset();
        auto& testset = byteIoTDataset.GetTestset();
    
        groundnut::ByteIoTClassifier clf;
    
        // train
        //Instrumentor::Get().BeginSession("Train", outFolder + "train-time.json");
        clf.Train(&trainset);
        //Instrumentor::Get().EndSession();
    
        // predict
        //Instrumentor::Get().BeginSession("Predict", outFolder + "predict-time.json");
        ClassificationMetrics metric;
        clf.Predict(&testset, metric, false);
        //Instrumentor::Get().EndSession();
    
        // review and metric
        //Instrumentor::Get().BeginSession("Review", outFolder + "log-review-metric-time.json");
        // std::ofstream outStream(outFolder + configBurstClf.ToString() \
        // + configBurstDataset.ToString() + "reviewBook.txt");
        // outStream << reviewBook.ToString();
        // outStream.close();
    
        std::ofstream outMetric(outFolder + config.ToString() + "byteiot-metrics.txt");
        std::cout << "avgDeviceInstance" << avgDeviceInstance*0.5 << "hour" << std::endl;
        outMetric << "Average Device Time: " << std::to_string(avgDeviceInstance*0.5) << " hour" << std::endl;
        outMetric << ToString(metric);
        outMetric.close();
        Instrumentor::Get().EndSession();
    }
*/