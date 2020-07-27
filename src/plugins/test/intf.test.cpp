//
// Created by Stack on 7/27/20.
//
#include "../../external/catch2/catch.hpp"
#include "../oms/intf.hpp"

int dumb(){
    return 1;
}

TEST_CASE("dumb test","test"){
    REQUIRE(dumb()==1);
}