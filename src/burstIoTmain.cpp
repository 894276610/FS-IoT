#include "magic_enum.hpp"
#include "kpacket.h"
#include <iostream>

#include "boost-serialize.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "boClassifier.h"
#include "kburst.h"
#include "utils-metric.h"
#include <fstream>
#include "timer.h"

std::string baseFolder = "/media/kunling/BigE/";
std::string datasetName = "IOTBEHAV2021";
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

for(float trainRate = 0.01; trainRate <= 0.15; trainRate += 0.01)
{
  const groundnut::ConfigBurstDataset configBurstDataset{1800,trainRate, 0.3, 0.5, {50, {2,0}, {15,0}}};
  const groundnut::ConfigBurstClf configBurstClf{10,100,50,500,1000,0.5,0.01};
  
  groundnut::BurstDataset burstDataset(datasetName, configBurstDataset);
  burstDataset.Load(pktDataset);
  burstDataset.TrainTestSplit();
  auto& trainset = burstDataset.GetTrainset();
  auto& testset = burstDataset.GetTestset();

  groundnut::BoClassifier boclf(configBurstClf);

  // train
  //Instrumentor::Get().BeginSession("Train", outFolder + "train-time.json");
  boclf.Train(&trainset);
  //Instrumentor::Get().EndSession();
  
  // predict
  //Instrumentor::Get().BeginSession("Predict", outFolder + "predict-time.json");
  ClassificationMetrics metric;
  groundnut::ReviewBook reviewBook = boclf.Predict(&testset, metric, false);
  //Instrumentor::Get().EndSession();

  // review and metric
  // Instrumentor::Get().BeginSession("Review", outFolder + configBurstClf.ToString() \
  // + configBurstDataset.ToString() + "log-review-metric-time.json");
  // std::ofstream outStream(outFolder + configBurstClf.ToString() \
  // + configBurstDataset.ToString() + "reviewBook.txt");
  // outStream << reviewBook.ToString();
  // outStream.close();

  std::ofstream outMetric(outFolder + configBurstClf.ToString() \
  + configBurstDataset.ToString() + "metrics.txt");
  outMetric << ToString(metric);
  outMetric.close();
  // Instrumentor::Get().EndSession();
}
 
}