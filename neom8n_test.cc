//
// Created by Pieter Bouwer on 2019/11/17.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "neom8n.h"

TEST_CASE("get sentence type") {
    SECTION("GSV sentence type - valid") {
        try {
            auto t = neom8n::GetSentenceType("$GPGSV,3,3,11,22,34,124,26,28,87,220,,30,22,332,10*48");
            REQUIRE(t == neom8n::GSV_TYPE);
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("valid sentence");
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            FAIL("valid/supported type");
        }
    }SECTION("GSV sentence type - valid (with newline)") {
        try {
            auto t = neom8n::GetSentenceType("$GPGSV,3,3,11,22,34,124,26,28,87,220,,30,22,332,10*48\n");
            REQUIRE(t == neom8n::GSV_TYPE);
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("valid sentence");
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            FAIL("valid/supported type");
        }
    }SECTION("GGA sentence type - invalid format") {
        try {
            auto t = neom8n::GetSentenceType(",074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("must throw exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        } catch (const neom8n::NoMatchingSentenceTypeError &e) {
            FAIL("incorrect exception");
        }
    }SECTION("GGA sentence type - invalid/unsupported type") {
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

TEST_CASE("parse GGA sentence") {
    SECTION("valid sentence") {
        try {
            auto gga = neom8n::GGA("$GNGGA,200107.000,2606.1668,S,02759.6537,E,1,08,1.2,1584.9,M,0.0,M,,*54");
            REQUIRE(gga.Type == neom8n::GGA_TYPE);
            REQUIRE(gga.Talker == "GN");
            REQUIRE(gga.Time == "200107.000");
            REQUIRE(gga.Latitude == "2606.1668");
            REQUIRE(gga.NorthSouthIndicator == "S");
            REQUIRE(gga.Longitude == "02759.6537");
            REQUIRE(gga.EastWestIndicator == "E");
            REQUIRE(gga.QualityIndicator == "1");
            REQUIRE(gga.NumberOfSatellitesUsed == "08");
            REQUIRE(gga.HDOP == "1.2");
            REQUIRE(gga.Altitude == "1584.9");
            REQUIRE(gga.GeoIDSeparation == "0.0");
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("must be able to parse a valid sentence");
        }
    }SECTION("valid sentenc (with newline)") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53\n");
            REQUIRE(gga.Type == neom8n::GGA_TYPE);
            REQUIRE(gga.Talker == "GN");
            REQUIRE(gga.Time == "074332.000");
            REQUIRE(gga.Latitude == "2606.1722");
            REQUIRE(gga.NorthSouthIndicator == "S");
            REQUIRE(gga.Longitude == "02759.6365");
            REQUIRE(gga.EastWestIndicator == "E");
            REQUIRE(gga.QualityIndicator == "1");
            REQUIRE(gga.NumberOfSatellitesUsed == "05");
            REQUIRE(gga.HDOP == "3.0");
            REQUIRE(gga.Altitude == "1577.4");
            REQUIRE(gga.GeoIDSeparation == "0.0");
        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("must be able to parse a valid sentence");
        }
    }SECTION("invalid sentence - no time") {
        try {
            auto gga = neom8n::GGA("$GNGGA,,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }SECTION("invalid sentence - no latitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }SECTION("invalid sentence - no longitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,,E,1,05,3.0,1577.4,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }SECTION("invalid sentence - no altitude") {
        try {
            auto gga = neom8n::GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,,M,0.0,M,,*53");
            FAIL("shoud have thown an exception");
        } catch (const neom8n::InvalidSentenceError &e) {
            SUCCEED();
        }
    }
}

TEST_CASE("parse GSV sentence") {
    SECTION("valid sentence") {
        try {
            auto gsv = neom8n::GSV("$GPGSV,3,2,10,09,23,131,30,12,30,276,,13,17,356,,17,26,037,05*75");
            REQUIRE(gsv.Type == neom8n::GSV_TYPE);
            REQUIRE(gsv.Talker == "GP");
            REQUIRE(gsv.NumberOfMessages == "3");
            REQUIRE(gsv.MessageNumber == "2");
            REQUIRE(gsv.NumberOfSatellites == "10");
            REQUIRE(gsv.SatelliteInfos.size() == 4);

            REQUIRE(gsv.SatelliteInfos[0].SatelliteID == "09");
            REQUIRE(gsv.SatelliteInfos[0].Elevation == "23");
            REQUIRE(gsv.SatelliteInfos[0].Azimuth == "131");
            REQUIRE(gsv.SatelliteInfos[0].SignalStrength == "30");

            REQUIRE(gsv.SatelliteInfos[1].SatelliteID == "12");
            REQUIRE(gsv.SatelliteInfos[1].Elevation == "30");
            REQUIRE(gsv.SatelliteInfos[1].Azimuth == "276");
            REQUIRE(gsv.SatelliteInfos[1].SignalStrength == "");

            REQUIRE(gsv.SatelliteInfos[2].SatelliteID == "13");
            REQUIRE(gsv.SatelliteInfos[2].Elevation == "17");
            REQUIRE(gsv.SatelliteInfos[2].Azimuth == "356");
            REQUIRE(gsv.SatelliteInfos[2].SignalStrength == "");

            REQUIRE(gsv.SatelliteInfos[3].SatelliteID == "17");
            REQUIRE(gsv.SatelliteInfos[3].Elevation == "26");
            REQUIRE(gsv.SatelliteInfos[3].Azimuth == "037");
            REQUIRE(gsv.SatelliteInfos[3].SignalStrength == "05");

        } catch (const neom8n::InvalidSentenceError &e) {
            FAIL("must be able to parse a valid sentence");
        }
    }
}


