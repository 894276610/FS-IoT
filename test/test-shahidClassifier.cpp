#include "shahidClassifier.h"
#include <catch2/catch_test_macros.hpp>
#include "byteIoTClassifier.h"
#include "packetDataset.h"
#include "burstDataset.h"
#include "burstClassifier.h"
#include "byteIoTDataset.h"

std::filesystem::path pcapPath = "/home/kunling/BurstIoT/test/pcap-test";
std::filesystem::path serializePath = "/home/kunling/BurstIoT/test/ikettle-serial.burstDataset";

using namespace groundnut;

TEST_CASE("byteiot classifier could init", "[single-file]")
{
    // Setup device and test packets
    groundnut::PacketDataset packetDataset("Aalto-partial");
    packetDataset.AddTragetDevice("5c:cf:7f:06:d9:02", "iKettle2");
    packetDataset.AddTragetDevice("3c:49:37:03:17:db", "ednet-cam1");

    packetDataset.LoadPcap(pcapPath);
    
    ShahidDataset shahidDataset("Aalto-partial", 6, 1800, 600);
    shahidDataset.Load(packetDataset);

    std::unordered_map<uint16_t, ShahidSlots> trainSet, testSet;
    shahidDataset.TrainTestSplit(trainSet, testSet, 30);

    ShahidClassifier clf(shahidDataset.devicesVec);
    
    clf.Train(&trainSet);

    for(auto& [deviceId, instances]: testSet)
    {
        for(auto& instance: instances)
        {
            std::cout << "prediction:" << clf.Predict(instance) << "\n";
            std::cout << "actual:" << instance.deviceId << "\n";
        }
    }
}