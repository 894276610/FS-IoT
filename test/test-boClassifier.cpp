#include "boClassifier.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "packetDataset.h"
#include "burstDataset.h"

namespace groundnut
{


TEST_CASE("bo classifier", "[single-file]")
{
    std::filesystem::path pcapPath = "/home/kunling/BurstIoT/test/pcap-test";
    std::filesystem::path serializePath = "/home/kunling/BurstIoT/test/ikettle-serial.burstDataset";
      
    // prepare train set
    groundnut::PacketDataset packetDataset(groundnut::DatasetEnum::Aalto_TEST);
    packetDataset.AddTragetDevice("5c:cf:7f:06:d9:02", "iKettle");
    packetDataset.AddTragetDevice("3c:49:37:03:17:db", "ednet-cam1");
    packetDataset.LoadPcap(pcapPath);

    BurstDataset burstDataset(groundnut::DatasetEnum::Aalto_TEST);
    BurstTrh trh{50, {2,0}, {15,0}};

    burstDataset.Load(packetDataset);
    burstDataset.TrainTestSplit();

    FsClfConfig cfg;
    BoClassifier boclf(cfg);

    auto& trainset = burstDataset.GetTrainset();
    boclf.Train(&trainset);
    
    auto& testSet = burstDataset.GetTestset();

    for(auto& [deviceId, bgroups]: testSet)
    {
        for(auto& bgroup : bgroups)
        {
            std::string result;
            boclf.Predict(bgroup, result, false);
            std::cout << "prediction:" << result << "\n";
            std::cout << "actual:" << bgroup[0]->GetLabel()<< "\n";
            
        }
    }

}
}