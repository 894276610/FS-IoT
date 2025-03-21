#include <catch2/catch_test_macros.hpp>
#include "packetDataset.h"
#include "PcapFileDevice.h"
#include "burstDataset.h"
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
    

    BurstDataset burstDataset("Aalto-partial");
    BurstTrh trh{50, {2,0}, {15,0}};

    burstDataset.Load(packetDataset);
    REQUIRE(burstDataset.GetBurstTrh().inTrh.tv_nsec == trh.inTrh.tv_nsec);
    REQUIRE(burstDataset.GetDevicesVec()[0].GetLabel() == "iKettle2");
    REQUIRE(burstDataset.GetName() == "Aalto-partial");

    // August 17 iKettle
    
    REQUIRE(burstDataset.GetRawMap()[0][0].size() == 3); //deviceId = 0, slotindex = 0 has 2 bursts
    REQUIRE(burstDataset.GetRawMap()[0][0][0]->GetPktNum() == 10); //first one contains 10 pkts
    REQUIRE(burstDataset.GetRawMap()[0][0][1]->GetPktNum() == 1);  //second one contains 1 pkt
    REQUIRE(burstDataset.GetRawMap()[0][0][2]->GetPktNum() == 1);  //third one contains 1 pkt

    // Sep 7 iKettle
    REQUIRE(burstDataset.GetRawMap()[0][1].size() == 2); //deviceId = 0, slotindex = 0 has 2 bursts
    REQUIRE(burstDataset.GetRawMap()[0][1][0]->GetPktNum() == 10); //first one contains 10 pkts
    REQUIRE(burstDataset.GetRawMap()[0][1][1]->GetPktNum() == 1);  //second one contains 1 pkt
    REQUIRE(burstDataset.GetSlotDuration() == 1800);

    // Sep 6 cam
    REQUIRE(burstDataset.GetRawMap()[1][0].size() == 2); //deviceId = 1, slotindex = 0 has 2 bursts
    REQUIRE(burstDataset.GetRawMap()[1][0][0]->GetPktNum() == 2); //first one contains 10 pkts
    REQUIRE(burstDataset.GetRawMap()[1][0][1]->GetPktNum() == 20);  //second one contains 1 pkt

    // Sep 7 cam
    REQUIRE(burstDataset.GetRawMap()[1][1].size() == 2);
    REQUIRE(burstDataset.GetRawMap()[1][1][0]->GetPktNum() == 20); //first one contains 10 pkts
    REQUIRE(burstDataset.GetRawMap()[1][1][1]->GetPktNum() == 1);  //second one contains 1 pkt

    groundnut::Serialize(burstDataset, serializePath);

    BurstDataset burstDataset2;
    groundnut::LoadBin(burstDataset2, serializePath);

    REQUIRE(burstDataset2.GetName() == "Aalto-partial");
    REQUIRE(burstDataset2.GetRawMap()[0][1].size() == 2); //deviceId = 0, slotindex = 1 has 2 bursts
    REQUIRE(burstDataset2.GetRawMap()[0][1][0]->GetPktNum() == 10); //first one contains 10 pkts
    REQUIRE(burstDataset2.GetRawMap()[0][1][1]->GetPktNum() == 1);  //second one contains 1 pkt

    burstDataset2.TrainTestSplit();

    REQUIRE(burstDataset2.GetTrainset()[0].size() ==1);
    REQUIRE(burstDataset2.GetTestset()[0].size() == 1);
    REQUIRE(burstDataset2.GetTrainset()[0][0].size() == 3);  // aug17, 3bursts as trainset
    REQUIRE(burstDataset2.GetTestset()[0][0].size() == 2);  // sep7, 2bursts as tesetset

}


}
