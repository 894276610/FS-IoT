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

std::string baseFolder = "/media/kunling/BigE/";
std::string datasetName = "UNSW201620";
std::string outFolder = baseFolder + datasetName + "/";
std::string mappingFolder = "/home/kunling/BurstIoT/mappings/";
std::string pktDatasetOutName = datasetName + ".pktDataset";

int main() {

    // init packet dataset and add target devices
    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");

    // load existing packet dataset or load from raw packets.
    if(std::filesystem::exists(outFolder + pktDatasetOutName))
    {
        pktDataset.LoadBin(outFolder + pktDatasetOutName);
    }
    else
    {
        pktDataset.LoadPcap(outFolder + "RAW");
        pktDataset.Serialize(outFolder+ pktDatasetOutName);  
    }
  
    // init burst dataset
    groundnut::ConfigBurstDataset config;

    for(float rate = 0.01; rate <=0.5; rate+=0.01)
    {
        config.trainRate= rate;
        groundnut::ByteIoTDataset byteIoTDataset(datasetName, config);

        byteIoTDataset.Load(pktDataset);
        byteIoTDataset.TrainTestSplit();
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
        outMetric << ToString(metric);
        outMetric.close();
        Instrumentor::Get().EndSession();
    }
    
   

 
}