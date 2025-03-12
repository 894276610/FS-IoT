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


enum class Color { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  
  groundnut::PacketDataset pktDataset("unsw2016");
  pktDataset.AddTragetDevices("/home/kunling/BurstIoT/mappings/unsw2016_device_mac_mappings.csv");
  //pktDataset.LoadPcap("/media/kunling/Research/UNSW2016/RAW");
  //pktDataset.Serialize("/media/kunling/Research/UNSW2016/unsw2016.pktDataset");
  pktDataset.LoadBin("/media/kunling/BigE/UNSW2016/unsw2016.pktDataset");
  
  groundnut::BurstDataset burstDataset("unsw2016");
  groundnut::BurstTrh trh{50,{2,0},{15,0}};
  burstDataset.Load(pktDataset, trh);

  burstDataset.TrainTestSplit(0.5);

  groundnut::BoClassifier boclf(50);

  auto& trainset = burstDataset.GetTrainset();
  auto& testset = burstDataset.GetTestset();

  boclf.Train(&trainset);
  ClassificationMetrics metric;

  std::cout << "training end" << std::endl;
  groundnut::ReviewBook reviewBook = boclf.Predict(&testset, metric, true);

  std::ofstream outStream("/media/kunling/BigE/UNSW2016/reviewBook.txt");
  outStream << reviewBook.ToString();
  outStream.close();

  std::ofstream outMetric("/media/kunling/BigE/UNSW2016/metrics.txt");
  outMetric << ToString(metric);
  outMetric.close();
}