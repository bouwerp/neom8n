//
// Created by Pieter Bouwer on 2019/11/17.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "neom8n.h"

TEST_CASE( "GGA sentence" ) {
    SECTION("valid sentence") {
        try {
            auto gga = GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            REQUIRE(gga.Type == GGA_TYPE);
            REQUIRE(gga.Talker == "GN");
            REQUIRE(gga.Time == "074332.000");
            REQUIRE(gga.Latitude == 2606.1722);
            REQUIRE(gga.NorthSouthIndicator == "S");
            REQUIRE(gga.Longitude == 2759.6365);
            REQUIRE(gga.EastWestIndicator == "E");
            REQUIRE(gga.QualityIndicator == "1");
            REQUIRE(gga.NumberOfSatellitesUsed == 5);
            REQUIRE(gga.HDOP == 3.0);
            REQUIRE(gga.Altitude == 1577.4);
            REQUIRE(gga.GeoIDSeparation == 0.0);
        } catch (const InvalidSentenceError &e) {
            FAIL("must be able to parse a valid sentence");
        }
    }
    SECTION("invalid sentence - no time") {
        try {
            auto gga = GGA("$GNGGA,,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const InvalidSentenceError &e) {
            SUCCEED();
        }
    }
    SECTION("invalid sentence - no altitude") {
        try {
            auto gga = GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const InvalidSentenceError &e) {
            SUCCEED();
        }
    }
}


