#include <catch2/catch_test_macros.hpp>
#include "packetDataset.h"
#include "PcapFileDevice.h"
#include "pcpp-common.h"

// Setup: Create a temporary directory with a test .pcap file
std::filesystem::path pcapPath = "/home/kunling/BurstIoT/test/pcap-test";

// Create a test PCAP file with one Ethernet packet
TEST_CASE(" packetDataset init test", "[single-file]")
{
    // Setup device and test packets
    groundnut::PacketDataset dataset(groundnut::DatasetEnum::UNSW_TEST);
    dataset.AddTragetDevice("5c:cf:7f:06:d9:02", "bulb");
    dataset.LoadPcap(pcapPath);

    // CAPTURE(dataset.GetDatasetStat(), dataset.GetDataset());  
    REQUIRE(dataset.GetDataset().size() == 23);

    REQUIRE(dataset.GetDatasetStat().totalPktNum == 67);
    CHECK(dataset.GetDatasetStat().devStat[0].totalSend == 19);
    CHECK(dataset.GetDatasetStat().devStat[0].totalRecv == 4);

    CHECK(dataset.GetDevicesMap().size() == 1);
    REQUIRE(dataset.GetDevicesMap().at("5c:cf:7f:06:d9:02").GetLabel() == "bulb");
    REQUIRE(dataset.GetName() == groundnut::DatasetEnum::UNSW_TEST); 

    for(auto pPacketVec: dataset.GetRawPacketVec())
    {
        REQUIRE(pPacketVec == nullptr);
    }
}

TEST_CASE(" packetDataset devices load", "[single-file]")
{
    groundnut::PacketDataset dataset(groundnut::DatasetEnum::UNSW_TEST);
    dataset.AddTragetDevices(std::filesystem::path("/home/kunling/BurstIoT/mappings/UNSW201620_device_mac_mappings.csv"));

    REQUIRE(dataset.GetDevicesMap().size() == 22);
}