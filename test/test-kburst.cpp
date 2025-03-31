#include "kburst.h"
#include "kdevice.h"
#include "kpacket.h"
#include "time-utils.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include <optional>

using namespace groundnut;
TEST_CASE("kburst could be constructed empty and add a packet", "[single-file]")
{
    groundnut::KBurst burst;
    
    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "");
    REQUIRE(burst.GetFirstPktStamp() == timespec{0,0});
    REQUIRE(burst.GetPktNum() == 0);
    REQUIRE(burst.GetUniPktNum() == 0);
    REQUIRE(burst.GetCountMap().size() ==0);
    REQUIRE( burst.GetLastPktStamp() == timespec{0,0});

    KPacket onePacket{0,-90, 0xcdab, {10,0}};
    
    burst.AddPacket(onePacket);

    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    const auto& distribution = burst.GetCountMap();

    REQUIRE(burst.GetLabel() == "");
    REQUIRE(burst.GetFirstPktStamp() == timespec{10,0});
    REQUIRE(burst.GetLastPktStamp() == timespec{10,0});
    REQUIRE(burst.GetPktNum() == 1);
    REQUIRE(burst.GetUniPktNum() == 1);
    REQUIRE(burst.GetCountMap().size() ==1);

    KPacket anotherPacket{0,80, 0xcccc, {10,10}};
    burst.AddPacket(anotherPacket);

    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "");
    REQUIRE(burst.GetFirstPktStamp() == timespec{10,0});
    REQUIRE(burst.GetLastPktStamp() == timespec{10,10});
    REQUIRE(burst.GetPktNum() == 2);
    REQUIRE(burst.GetUniPktNum() == 2);
    REQUIRE(burst.GetCountMap().size() ==2);

    KPacket thirdPacket{0,80, 0xccdc, {3,100}};
    burst.AddPacket(thirdPacket);

    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(),
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "");
    REQUIRE(burst.GetFirstPktStamp() == timespec{3,100});
    REQUIRE(burst.GetLastPktStamp()  == timespec{10,10});
    REQUIRE(burst.GetPktNum() == 3);
    REQUIRE(burst.GetUniPktNum() == 2);
    REQUIRE(burst.GetCountMap().size() ==2);
}

TEST_CASE("kburst could be constructed using label and packet insert \
    sequence does not matter with distribution.", "[single-file]")
{
    groundnut::KBurst burst("Amazon Echo");
    groundnut::KBurst burst1(burst);
    
    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "Amazon Echo");
    REQUIRE(burst.GetFirstPktStamp() == timespec{0,0});
    REQUIRE(burst.GetPktNum() == 0);
    REQUIRE(burst.GetUniPktNum() == 0);
    REQUIRE(burst.GetCountMap().size() ==0);
    REQUIRE(::operator==(burst.GetLastPktStamp(),timespec{0,0}));

    KPacket firstPkt{1, 34, 0xcc, timespec{1,1}};
    KPacket secondPkt{2, 35, 0xc1, timespec{3,3}};
    KPacket thirdPkt{3,45, 0xff, timespec{2,4}};

    burst.AddPacket(firstPkt);
    burst.AddPacket(secondPkt);
    burst.AddPacket(thirdPkt);

    burst1.AddPacket(thirdPkt);
    burst1.AddPacket(secondPkt);
    burst1.AddPacket(firstPkt);

    REQUIRE(burst.isEqualDistribution(burst1));
}

TEST_CASE("kburst could be constructed using device class", "[single-file]")
{
    KDevice belkinDevice("Belkin Wemo", "00:00:00:11:11:11");

    groundnut::KBurst burst(belkinDevice);
    
    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "Belkin Wemo");
    REQUIRE(burst.GetFirstPktStamp() == timespec{0,0});
    REQUIRE(burst.GetPktNum() == 0);
    REQUIRE(burst.GetUniPktNum() == 0);
    REQUIRE(burst.GetCountMap().size() ==0);
    REQUIRE( ::operator==(burst.GetLastPktStamp(),timespec{0,0}));
}

TEST_CASE("kburst could be constructed using a list of bursts", "[single-file]")
{
    KDevice belkinDevice("Belkin Wemo", "00:00:00:11:11:11");

    groundnut::KBurst burst(belkinDevice);
    
    CAPTURE(burst.GetLabel(), burst.GetUniPktNum(),
    burst.GetFirstPktStamp(), burst.GetLastPktStamp(), 
    burst.GetCountMap());

    REQUIRE(burst.GetLabel() == "Belkin Wemo");
    REQUIRE(burst.GetFirstPktStamp() == timespec{0,0});
    REQUIRE(burst.GetPktNum() == 0);
    REQUIRE(burst.GetUniPktNum() == 0);
    REQUIRE(burst.GetCountMap().size() ==0);
    REQUIRE(::operator==(burst.GetLastPktStamp(),timespec{0,0}));

    // prepare some packets
    KPacket firstPkt{0,10, 0xcdab, {1,10}};
    KPacket secondPkt{0,-50, 0xcdab, {2,20}};
    KPacket thirdPkt{0,10, 0xcdab, {3,40}};

    KPacket forthPkt{0,-50, 0xcdab, {5,90}};
    KPacket fifthPkt{0,90, 0xcdab, {6,70}};

    KPacket sixthPkt{0,-90, 0xcdab, {7,10}};
    KPacket seventhPkt{0,60, 0xcdab, {8,0}};
    KPacket eighthPkt{0,1500, 0xcdab, {10,0}};
    

    // add a list of bursts
    std::shared_ptr<KBurst> p_burst1 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst2 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst3 = std::make_shared<KBurst>();

    p_burst1->AddPacket(firstPkt);
    p_burst1->AddPacket(secondPkt);
    p_burst1->AddPacket(thirdPkt);

    p_burst2->AddPacket(forthPkt);
    p_burst2->AddPacket(fifthPkt);

    p_burst3->AddPacket(sixthPkt);
    p_burst3->AddPacket(seventhPkt);
    p_burst3->AddPacket(eighthPkt);

    std::vector<std::shared_ptr<KBurst>> burstVector;
    burstVector.push_back(p_burst1);
    burstVector.push_back(p_burst2);
    burstVector.push_back(p_burst3);

    KBurst bigBurst(burstVector);

    CAPTURE(bigBurst.GetLabel(), bigBurst.GetUniPktNum(),
    bigBurst.GetFirstPktStamp(), bigBurst.GetLastPktStamp(), 
    bigBurst.GetCountMap());

    REQUIRE(bigBurst.GetLabel() == "");
    REQUIRE(bigBurst.GetFirstPktStamp() == timespec{1,10});
    REQUIRE(bigBurst.GetLastPktStamp() == timespec{10,0});
    REQUIRE(bigBurst.GetPktNum() == 8);
    REQUIRE(bigBurst.GetCountMap().size() ==6);
    REQUIRE(bigBurst.GetUniPktNum() == 6);

    //std::cout << bigBurst.ToString();
}

TEST_CASE("kburst distance calculation test ", "[single-file]")
{
    // prepare some packets
    KPacket pkt1{0,10, 0xcdab, {1,10}}; //0.25
    KPacket pkt2{0,20, 0xcdab, {2,20}};
    KPacket pkt3{0,30, 0xcdab, {3,40}}; 
    KPacket pkt4{0,40, 0xcdab, {5,90}};
    
    KPacket pkt5{0,10, 0xcdab, {6,70}}; //0.25
    KPacket pkt6{0,20, 0xcdab, {7,10}};
    KPacket pkt7{0,50, 0xcdab, {8,0}};
    KPacket pkt8{0,60, 0xcdab, {10,0}};

    KBurst burst1 = KBurst();
    KBurst burst2 = KBurst();
    KBurst burst3 = KBurst();
    std::shared_ptr<KBurst> p_burst1 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst2 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst3 = std::make_shared<KBurst>();
    
    burst1.AddPacket(pkt1);
    burst1.AddPacket(pkt2);
    burst1.AddPacket(pkt3);
    burst1.AddPacket(pkt4);

    p_burst1->AddPacket(pkt1);
    p_burst1->AddPacket(pkt2);
    p_burst1->AddPacket(pkt3);
    p_burst1->AddPacket(pkt4);

    burst2.AddPacket(pkt5);
    burst2.AddPacket(pkt6);
    burst2.AddPacket(pkt7);
    burst2.AddPacket(pkt8);

    p_burst2->AddPacket(pkt5);
    p_burst2->AddPacket(pkt6);
    p_burst2->AddPacket(pkt7);
    p_burst2->AddPacket(pkt8);

    burst3.AddPacket(pkt1);
    burst3.AddPacket(pkt5);
    p_burst3->AddPacket(pkt1);
    p_burst3->AddPacket(pkt5);

    burst1.convert_to_frequency();
    burst2.convert_to_frequency();
    burst3.convert_to_frequency();
    p_burst1->convert_to_frequency();
    p_burst2->convert_to_frequency();
    p_burst3->convert_to_frequency();

    float distance = burst1.Distance(burst2);
    float manual_distance = 1/sqrt(2);

    REQUIRE(distance == manual_distance);

    distance = burst1.Distance(burst3);
    manual_distance = 1/sqrt(2);

    REQUIRE(distance == manual_distance);

    distance = p_burst1->Distance(p_burst2);
    REQUIRE(distance == manual_distance);

    distance = p_burst1->Distance(p_burst3);
    REQUIRE(distance == manual_distance);

}


TEST_CASE("Basic Read/Write", "[cache][single-thread]") {
    BurstCache cache;
    
    KPacket pkt1{0,10, 0xcdab, {1,10}}; //0.25
    KPacket pkt2{0,20, 0xcdab, {2,20}};
    KPacket pkt3{0,30, 0xcdab, {3,40}}; 
    KPacket pkt4{0,40, 0xcdab, {5,90}};
    
    KPacket pkt5{0,10, 0xcdab, {6,70}}; //0.25
    KPacket pkt6{0,20, 0xcdab, {7,10}};
    KPacket pkt7{0,50, 0xcdab, {8,0}};
    KPacket pkt8{0,60, 0xcdab, {10,0}};

    KBurst burst1 = KBurst();
    KBurst burst2 = KBurst();
    KBurst burst3 = KBurst();

    std::shared_ptr<KBurst> p_burst1 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst2 = std::make_shared<KBurst>();
    std::shared_ptr<KBurst> p_burst3 = std::make_shared<KBurst>();

    p_burst1->AddPacket(pkt1);
    p_burst1->AddPacket(pkt2);
    p_burst1->AddPacket(pkt3);
    p_burst1->AddPacket(pkt4);

    burst2.AddPacket(pkt5);
    burst2.AddPacket(pkt6);
    burst2.AddPacket(pkt7);
    burst2.AddPacket(pkt8);

    p_burst2->AddPacket(pkt5);
    p_burst2->AddPacket(pkt6);
    p_burst2->AddPacket(pkt7);
    p_burst2->AddPacket(pkt8);

    burst3.AddPacket(pkt1);
    burst3.AddPacket(pkt5);
    p_burst3->AddPacket(pkt1);
    p_burst3->AddPacket(pkt5);

    BurstVec bvec;
    
    SECTION("Write then Read") {
        cache.Write(burst1, {0.3, {p_burst1, p_burst2, p_burst3}});
        std::optional<SearchResult> result = cache.Read(burst1);
        REQUIRE(result.has_value());
        REQUIRE(result.value().minDistance == Catch::Approx(0.3).epsilon(0.01));
    }
    
    SECTION("Read Missing Key") {
        auto result = cache.Read(burst2);
        REQUIRE_FALSE(result.has_value());
    }
}

