#include "kdevice.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("kdevice could be constructed empty", "[single-file]")
{
    groundnut::KDevice device;
    CAPTURE(device.GetDevId(), device.GetLabel(), device.GetAddr());
    
    REQUIRE(device.GetDevId() == 0);
    REQUIRE(device.GetLabel() == "");
    REQUIRE(device.GetAddr().toString() == "00:00:00:00:00:00");

}

TEST_CASE("kdevice could be constructed properly by arguments", "[single-file]")
{
    groundnut::KDevice device("Amazon Echo", "0E:00:5C:12:4f:ac");
    CAPTURE(device.GetDevId(), device.GetLabel(), device.GetAddr());

    REQUIRE(device.GetDevId() == 1);
    REQUIRE(device.GetLabel() == "Amazon Echo");
    REQUIRE(device.GetAddr().toString() == "0e:00:5c:12:4f:ac");

}