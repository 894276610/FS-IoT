#include <catch2/catch_test_macros.hpp>
#include "packetDataset.h"
#include "PcapFileDevice.h"
#include "burstDataset.h"
#include "pcpp-common.h"
#include "kburst.h"
#include "serialize-utils.h"
#include "boost-serialize.h"
#include "time-utils.h"
#include "shahidDataset.h"

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
    
    ShahidDataset shahidDataset("Aalto-partial", 6, 1800, 600);
    shahidDataset.Load(packetDataset);

    REQUIRE(shahidDataset.devicesVec[0].GetLabel() == "iKettle2");
    REQUIRE(shahidDataset.name == "Aalto-partial");

    // Have generated 2 slots for each device
    REQUIRE(shahidDataset.GetData()[0].size() == 2); 
    REQUIRE(shahidDataset.GetData()[1].size() == 2); 

    // Each device has 3 streams?
    REQUIRE(shahidDataset.GetData()[0][0].streams.size() == 3); // ikettle2 没有IP层的, 五元组的计算都是0
    REQUIRE(shahidDataset.GetData()[0][1].streams.size() == 3); // ikettle2

    REQUIRE(shahidDataset.GetData()[1][0].streams.size() == 4); // ednet-cam1 sep7
    REQUIRE(shahidDataset.GetData()[1][1].streams.size() == 4); // ednet-cam1 sep7

    std::unordered_map<uint16_t, ShahidSlots> trainSet, testSet;
    shahidDataset.TrainTestSplit(trainSet, testSet, 30);

    REQUIRE(trainSet[1].size() == 1);
    REQUIRE(testSet[1].size() == 1);
    

    for(int i = 0; i < 22; i++)
    {
        std::cout << i << ":";
        std::cout << shahidDataset.GetData()[1][0].streams[3]->features.at(i);
        std::cout << ",";
    }
    
    std::cout << shahidDataset.GetData()[1][0].streams[3]->features.size();  

}


}
