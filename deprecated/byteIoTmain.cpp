#include "magic_enum.hpp"
#include "kpacket.h"
#include <iostream>

#include "burstClassifier.h"
#include "boost-serialize.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "boClassifier.h"
#include "kburst.h"
#include <fstream>
#include "timer.h"

#include "byteIoTClassifier.h"
#include "byteIoTDataset.h"

#include "labSetting.h"

void InstancePercentLab(LabSetting settings);
void HourBudgetLab(LabSetting settings);
void DivisionLab(LabSetting settings);

LabSetting GetDivisionSetting()
{
    LabSetting settings;
    settings.methodName = MethodEnum::BYTEIOT;
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = groundnut::DatasetEnum::BehavIoT2021;
    settings.independentArg = IndependentArgEnum::WINDOW_SIZE;
    settings.scenario = ExperimentEnum::BYTEIOT_SLOT_LENGTH;

    settings.slotDuration = 15;
    settings.start = 15; // sec slotduration
    settings.end = 300;
    settings.step = 15;
    settings.clfConfig.review = false;
    return settings;
}

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;
    settings.methodName = MethodEnum::BYTEIOT;
    settings.baseFolder = "/media/kunling/BigE/";
    settings.datasetName = groundnut::DatasetEnum::BehavIoT2021; //"IOTBEHAV2021";//"NEUKI2019"; // "UNSW201620";
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.scenario = ExperimentEnum::FEW_SHOTS;
    settings.burstTrh.inTrh = {2,0};
    settings.burstTrh.inTrhF = 2.0f;
    settings.trainRate = 0.5f;
    settings.start = 4200; // sec slotduration
    settings.end = 4800;
    settings.step = 300;
    settings.clfConfig.review = false;
    return settings;
}

int main() {
    
    LabSetting settings = GetFewShotSettingTemplate();

    // if(settings.experimentMode == "ipc")
    // {
    //     InstancePercentLab(settings);
    // }
    // else if(settings.experimentMode == "hbd")
    // {
    //     HourBudgetLab(settings);
    // }
    // else if(settings.experimentMode == "division")
    // {
    //     DivisionLab(settings);
    // }
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