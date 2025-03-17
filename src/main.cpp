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
  groundnut::BurstDataset burstDataset(datasetName);
  groundnut::BurstTrh trh{50,{2,0},{15,0}};
  burstDataset.Load(pktDataset, trh);

  burstDataset.TrainTestSplit();
  groundnut::ConfigBurstClf configBurstClf;
  groundnut::BoClassifier boclf(configBurstClf);

  // divide
  auto& trainset = burstDataset.GetTrainset();
  auto& testset = burstDataset.GetTestset();

  // train
  Instrumentor::Get().BeginSession("Train", outFolder + "train-time.json");
  boclf.Train(&trainset);
  Instrumentor::Get().EndSession();
  ClassificationMetrics metric;

  // predict
  Instrumentor::Get().BeginSession("Predict", outFolder + "predict-time.json");
  groundnut::ReviewBook reviewBook = boclf.Predict(&testset, metric, true);
  Instrumentor::Get().EndSession();

  // review and metric
  Instrumentor::Get().BeginSession("Review", outFolder + "log-review-metric-time.json");
  std::ofstream outStream(outFolder + "reviewBook.txt");
  outStream << reviewBook.ToString();
  outStream.close();

  std::ofstream outMetric(outFolder + "metrics.txt");
  outMetric << ToString(metric);
  outMetric.close();
  Instrumentor::Get().EndSession();
}