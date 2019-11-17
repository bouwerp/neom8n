//
// Created by Pieter Bouwer on 2019/11/17.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "neom8n.h"

TEST_CASE( "parse valid GGA sentence" ) {
    try {
        auto gga = GGA("$GNGGA,074332.000,2606.1722,S,02759.6365,E,1,05,3.0,1577.4,M,0.0,M,,*53");
    } catch (const InvalidSentenceError &e) {
        FAIL("must be able to parse a valid sentence");
    }
}


