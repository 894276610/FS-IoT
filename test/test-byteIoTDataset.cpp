#include <catch2/catch_test_macros.hpp>
#include "packetDataset.h"
#include "PcapFileDevice.h"
#include "byteIoTDataset.h"
#include "pcpp-common.h"
#include "kburst.h"
#include "serialize-utils.h"
#include "boost-serialize.h"
#include "time-utils.h"

std::filesystem::path pcapPath = "/home/kunling/BurstIoT/test/pcap-test";
std::filesystem::path serializePath = "/home/kunling/BurstIoT/test/ikettle-serial.burstDataset";

namespace groundnut{

TEST_CASE(" packetDataset init test", "[single-file]")
{
    // Setup device and test packets
    groundnut::PacketDataset packetDataset("Aalto-partial");
    packetDataset.AddTragetDevice("5c:cf:7f:06:d9:02", "iKettle2");
    packetDataset.AddTragetDevice("3c:49:37:03:17:db", "ednet-cam1");

    packetDataset.LoadPcap(pcapPath);
    
    ConfigBurstDataset config;
    ByteIoTDataset byteIoTDataset("Aalto-partial", config);
    BurstTrh trh{50, {2,0}, {15,0}};

    byteIoTDataset.Load(packetDataset);
    REQUIRE(byteIoTDataset.GetDevicesVec()[0].GetLabel() == "iKettle2");
    REQUIRE(byteIoTDataset.GetName() == "Aalto-partial");

    // August 17 iKettle   
    REQUIRE(byteIoTDataset.GetRawMap()[0][0]->GetPktNum() == 12); //deviceId = 0, instanceIndex = 0 has 12 packets

    // Sep 7 iKettle
    REQUIRE(byteIoTDataset.GetRawMap()[0][1]->GetPktNum() == 11); //deviceId = 0, slotindex = 0 has 2 bursts

    // Sep 6 cam
    REQUIRE(byteIoTDataset.GetRawMap()[1][0]->GetPktNum() == 22); //deviceId = 1, slotindex = 0 has 2 bursts

    // Sep 7 cam
    REQUIRE(byteIoTDataset.GetRawMap()[1][1]->GetPktNum() == 21);

    byteIoTDataset.TrainTestSplit();

    REQUIRE(byteIoTDataset.GetTrainset()[0].size() == 1);
    REQUIRE(byteIoTDataset.GetTestset()[0].size() == 1);  
}
}
