#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include "burstMaker.h"
#include "kdevice.h"

using namespace groundnut;

using PacketVector = std::vector<const KPacket*>;

TEST_CASE("burst maker innerdelta and outerdelta test", "[single-file]")
{
    KDevice device = KDevice("bulb", "00:00:01:02:03:04");

    KPacket pkt1{0,10, 0xcdab, {1,0}}; 
    KPacket pkt2{0,20, 0xcdab, {2,500000001}}; //1.5
    KPacket pkt3{0,30, 0xcdab, {4,500000002}}; //2

    KPacket pkt4{0,40, 0xcdab, {7,3}};         //2.5  
    KPacket pkt5{0,10, 0xcdab, {10,4}};        //3 

    KPacket pkt6{0,10, 0xcdab, {13,500000005}};//3.5
    
    const PacketVector packetVector{&pkt1, &pkt2, &pkt3, &pkt4, &pkt5, &pkt6};

    BurstTrh trh{10, {2,0}, {15,0}};

    BurstMaker maker(&packetVector, device, trh);
    BurstVec bvec = maker.MakeBursts();

    REQUIRE(bvec.size() == 5);
    REQUIRE(bvec[0]->GetPktNum() == 2);
    REQUIRE(bvec[1]->GetPktNum() == 1);
    REQUIRE(bvec[0]->GetLabel() == "bulb");
    REQUIRE(bvec[1]->GetLabel() == "bulb");
    REQUIRE(bvec[0]->GetUniPktNum() == 2);
    REQUIRE(bvec[0]->GetFirstPktStamp() == timespec{1,0});
    REQUIRE(bvec[0]->GetLastPktStamp() == timespec{2,500000001});
    REQUIRE(bvec[0]->GetCountMap().size() == 2);
    REQUIRE(bvec[0]->GetCountUmap().size() == 2);

    BurstTrh trh1{10, {6,0}, {4,0}};

    BurstMaker maker1(&packetVector, device, trh1);
    bvec = maker1.MakeBursts();
    REQUIRE(bvec.size() == 3);
    REQUIRE(bvec[0]->GetPktNum() == 3);
    REQUIRE(bvec[1]->GetPktNum() == 2);
    REQUIRE(bvec[0]->GetLabel() == "bulb");
    REQUIRE(bvec[1]->GetLabel() == "bulb");
    REQUIRE(bvec[0]->GetUniPktNum() == 3);
    REQUIRE(bvec[0]->GetFirstPktStamp() == timespec{1,0});
    REQUIRE(bvec[0]->GetLastPktStamp() == timespec{4,500000002});
    REQUIRE(bvec[0]->GetCountMap().size() == 3);
    REQUIRE(bvec[0]->GetCountUmap().size() == 3);

}

TEST_CASE("burst maker max unique packet number test", "[single-file]")
{
    std::vector<const KPacket*> packetVector;
    KDevice device = KDevice("bulb", "00:00:01:02:03:04");

    KPacket pkt1{0,10, 0xcdab, {1,0}}; 
    KPacket pkt2{0,20, 0xcdab, {2,500000001}}; //1.5
    KPacket pkt3{0,30, 0xcdab, {4,500000002}}; //2
    KPacket pkt4{0,10, 0xcdab, {7,3}};         //2.5  
    KPacket pkt5{0,10, 0xcdab, {10,4}};        //3 
    KPacket pkt6{0,80, 0xcdab, {13,500000005}};//3.5
    
    packetVector.push_back(&pkt1);
    packetVector.push_back(&pkt2);
    packetVector.push_back(&pkt3);
    packetVector.push_back(&pkt4);
    packetVector.push_back(&pkt5);
    packetVector.push_back(&pkt6);

    BurstTrh trh{2, {12,0}, {15,0}};

    BurstMaker maker(&packetVector, device, trh);
    BurstVec bvec = maker.MakeBursts();
    
    REQUIRE(bvec.size() == 3);
    REQUIRE(bvec[0]->GetPktNum() == 2);
    REQUIRE(bvec[1]->GetPktNum() == 3);
    REQUIRE(bvec[0]->GetUniPktNum() == 2);
    REQUIRE(bvec[0]->GetCountMap().size() == 2);
    REQUIRE(bvec[0]->GetCountUmap().size() == 2);

}

TEST_CASE("burst maker long burst flow test", "[single-file]")
{
    std::vector<const KPacket*> packetVector;
    KDevice device = KDevice("bulb", "00:00:01:02:03:04");

    KPacket pkt1{0,10, 0xcdaa, {1,0}}; 
    KPacket pkt2{0,20, 0xcdab, {2,500000001}}; //1.5
    KPacket pkt3{0,30, 0xcdac, {3,500000002}}; //2
    KPacket pkt4{0,10, 0xcdaa, {3,6}};         //2.5  
    KPacket pkt5{0,10, 0xcdab, {4,4}};        //3 
    KPacket pkt6{0,80, 0xcdac, {5,500000005}};//3.5
    
    packetVector.push_back(&pkt1);
    packetVector.push_back(&pkt2);
    packetVector.push_back(&pkt3);
    packetVector.push_back(&pkt4);
    packetVector.push_back(&pkt5);
    packetVector.push_back(&pkt6);

    BurstTrh trh{10, {12,0}, {4,0}};

    BurstMaker maker(&packetVector, device, trh);
    BurstVec bvec = maker.MakeBursts();
    
    REQUIRE(bvec.size() == 4);
    REQUIRE(bvec[0]->GetPktNum() == 2);
    REQUIRE(bvec[1]->GetPktNum() == 2);
    REQUIRE(bvec[0]->GetUniPktNum() == 1);
    REQUIRE(bvec[0]->GetCountMap().size() == 1);
    REQUIRE(bvec[0]->GetCountUmap().size() == 1);
}