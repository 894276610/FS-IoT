#include "kpacket.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "KPacket could be construct and initialied properly", "[single-file]" ) 
{
    groundnut::KPacket packet;

    REQUIRE( packet.deviceId == 0 ); 
    REQUIRE( packet.signedLen == 0);
    REQUIRE( packet.timestamp.tv_nsec == 0);
    REQUIRE( packet.timestamp.tv_nsec == 0);
    REQUIRE( packet.hash5tuple == 0);
}

TEST_CASE(" KPacket could be compared properly", "[single-file]")
{
    groundnut::KPacket firstPacket(0, 100, 32, {20, 300});
    groundnut::KPacket secondPacket(0, -100, 29, {23, 100});

    REQUIRE( firstPacket < secondPacket);
    CAPTURE( firstPacket.deviceId);   
}