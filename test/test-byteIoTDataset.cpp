#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
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
    groundnut::PacketDataset packetDataset("Aalto_TEST");
    packetDataset.AddTragetDevice("5c:cf:7f:06:d9:02", "iKettle2");
    packetDataset.AddTragetDevice("3c:49:37:03:17:db", "ednet-cam1");

    packetDataset.LoadPcap(pcapPath);
    
    ConfigBurstDataset config;
    config.slotDuration = 1800;
	config.trainRate = 0.15;
	config.trainBudget = 10000; // by minute
	config.testRate = 0.5;

    BurstTrh trh{50, 2.0, 15.0};
    config.burstTrh = trh;
    ByteIoTDataset byteIoTDataset("Aalto_TEST", config);

    byteIoTDataset.Load(packetDataset);
    REQUIRE(byteIoTDataset.GetDevicesVec()[0].GetLabel() == "iKettle2");
    REQUIRE(byteIoTDataset.GetName() == "Aalto_TEST");

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

    // test the dataset metric of deviceId = 0, i.e. iKettle2
    REQUIRE(byteIoTDataset.GetRawMap()[0].size() == 2); //deviceId = 0, ikettle2 has 2 samples
    DivMetric metric = byteIoTDataset.GenDivMetric("iKettle2", byteIoTDataset.GetRawMap()[0]);
    
    // entropy = 0.5log2 + 0.5log2 = log2 = log_{10}^{2} = approx(0.3)
    REQUIRE(metric.entropy == Catch::Approx(0.3).epsilon(0.01) );

    // ip based? time = 7.3 + 35.6 = 43s packetNum = 12+11 = 23
    // diversity = 135, 113, 350, 342, 42,46 = size = 6
    REQUIRE(metric.repeatRate == 0);
    REQUIRE(metric.diversity== 6);
    REQUIRE(metric.burstRate == Catch::Approx(0.53).epsilon(0.01));

    std::cout << metric.ToString();

    metric = byteIoTDataset.GenDivMetric("ednet-cam1", byteIoTDataset.GetRawMap()[1]);
}
}
