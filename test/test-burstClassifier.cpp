#include "burstClassifier.h"
#include "packetDataset.h"
#include "burstDataset.h"

#include <catch2/catch_test_macros.hpp>

/*
Ednet-cam1, 9.6 C-9 Train 
Ednet-cam1, 9.7 A-1 Test
iKettle,   8.17 C-3 Train
iKettle,    9.7 A-1 Test
*/
namespace groundnut{

std::filesystem::path pcapPath = "/home/kunling/BurstIoT/test/pcap-test";
std::filesystem::path serializePath = "/home/kunling/BurstIoT/test/ikettle-serial.burstDataset";


TEST_CASE("burstClassifier init", "[single-file]")
{
    // prepare train set
    groundnut::PacketDataset packetDataset("Aalto-partial");
    packetDataset.AddTragetDevice("5c:cf:7f:06:d9:02", "iKettle");
    packetDataset.AddTragetDevice("3c:49:37:03:17:db", "ednet-cam1");
    packetDataset.LoadPcap(pcapPath);

    BurstDataset burstDataset("Aalto-partial");
    BurstTrh trh{50, {2,0}, {15,0}};

    burstDataset.Load(packetDataset, trh);
    burstDataset.TrainTestSplit(0.5);

    REQUIRE(burstDataset.GetTrainset().size() == 2);
    REQUIRE(burstDataset.GetTestset().size() == 2);
  
    REQUIRE(burstDataset.GetTrainset()[0].size() == 1);
    REQUIRE(burstDataset.GetTrainset()[1].size() == 1);

    // iKettle train instance contains 3 bursts.
    REQUIRE(burstDataset.GetTrainset()[0][0].size() == 3);

    // ednet train instance contains 2 bursts;
    REQUIRE(burstDataset.GetTrainset()[1][0].size() == 2);
    
    REQUIRE(burstDataset.GetTestset()[0].size() == 1);
    REQUIRE(burstDataset.GetTestset()[0].size() == 1);

    // iKettle train instance contains 2 bursts.
    REQUIRE(burstDataset.GetTestset()[0][0].size() == 2);

    // ednet train instance contains 2 bursts;
    REQUIRE(burstDataset.GetTestset()[1][0].size() == 2);
    
    BurstClassifier clf(50);
    auto& trainset = burstDataset.GetTrainset();
    clf.Train(&trainset);

    auto& testSet = burstDataset.GetTestset();
    
    for(auto& [deviceId, bgroups]: testSet)
    {
        for(auto& bgroup : bgroups)
        {
            for(auto& pBurst : bgroup)
            {
                const SearchResult& prediction = clf.Predict(pBurst);
                std::cout << prediction.minDistance << " ";
                std::cout << "prediction:" << prediction.nearTrainBursts[0]->GetLabel()<< "\n";
                std::cout << "actual:" << pBurst->GetLabel()<< "\n";
            }
        }
    }

}

}