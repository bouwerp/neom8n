//
// Created by Pieter Bouwer on 2019/11/17.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "neom8n.h"

TEST_CASE("get sentence type") {
    SECTION("GGA sentence type - valid") {
        try {
            auto t = neom8n::GetSentenceType("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            REQUIRE(t == neom8n::GGA_TYPE);
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("valid sentence");
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            FAIL("valid/supported type");
        }
    }
    SECTION("GGA sentence type - invalid format") {
        try {
            auto t = neom8n::GetSentenceType(",074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("must throw exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            FAIL("incorrect exception");
        }
    }
    SECTION("GGA sentence type - invalid/unsupported type") {
        try {
            auto t = neom8n::GetSentenceType("$GNABC,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("must throw exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("incorrect exception");
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            SUCCEED();
        }
    }
}

TEST_CASE( "parse GGA sentence" ) {
    SECTION("valid sentence") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            REQUIRE(gga.Type == neom8n::GGA_TYPE);
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
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("must be able to parse a valid sentence");
        }
    }
    SECTION("invalid sentence - no time") {
        try {
            auto gga = neom8n::GGA("$GNGGA,,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }
    SECTION("invalid sentence - no latitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }
    SECTION("invalid sentence - no longitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }
    SECTION("invalid sentence - no altitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }
}


