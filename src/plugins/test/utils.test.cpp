//
// Created by Stack on 7/27/20.
//

#include "../../external/catch2/catch.hpp"
#include "db.hpp"
#include "../common.hpp"
#include "utils.hpp"
#include "intfutils.hpp"
#include <string>
#include <utility>

using namespace std;
using namespace oms;
TEST_CASE("utils show work properly"){
    using rc=oms::rc;
    string afintf="host-vppout";
    string invalid_afintf="host-";
    string intf="intfname";
    SECTION("is_af_intf"){
        REQUIRE(is_af_intf(afintf));
        REQUIRE_FALSE(is_af_intf(invalid_afintf));
    }
    SECTION("extract intf name"){
        string extracted_intf;
        REQUIRE(extract_intf_name(afintf,extracted_intf)==rc::success);
        REQUIRE(extracted_intf=="vppout");
        REQUIRE(extract_intf_name(invalid_afintf,intf)==rc::err_invalid_arg);
    }
}

