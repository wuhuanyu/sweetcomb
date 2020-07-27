//
// Created by Stack on 7/27/20.
//
#include "../../external/catch2/catch.hpp"
#include "db.hpp"
#include "common.hpp"
#include "intf.hpp"
#include "routing.hpp"
#include <vom/om.hpp>
#include <iostream>

int dumb(){
    return 1;
}

using namespace std;

void sleep_for_next_test(){
    sleep(2);
    std::cout<<"sleeping prepare for next connection"<<std::endl;
    VOM::HW::disconnect();
    sleep(10);
}

TEST_CASE("lib oms should work properly","[oms]"){
//    using namespace oms::intf;
//    using namespace oms;
//    using namespace oms::routing;
//    std::system("ip link del vpphost");
//    std::system("ip link add vpphost type veth peer name vppout");
//    sleep(2);
//    std::cout<<"restarting"<<std::endl;
//    std::system("systemctl restart vpp");
//    sleep(2);
//    VOM::HW::init();
//    VOM::OM::init();
//    while (!VOM::HW::connect());
//    VOM::OM::populate("boot");

//    SECTION("create af packet interface and set ip"){
//        REQUIRE(create_af_packet_intf("host-vppout")==oms::rc::success);
//        REQUIRE(create_af_packet_intf("host-vpphost",true)==oms::rc::success);
//        SECTION("del af packet interface host-vppout"){
//            REQUIRE(del_af_packet_intf("host-vppout")==oms::rc::success);
//        }
//        SECTION("add af packet interface host-vppout and set ip"){
//            REQUIRE(create_af_packet_intf("host-vppout",true)==oms::rc::success);
//            REQUIRE(set_ip("host-vppout","10.0.1.1/24")==oms::rc::success);
//
//            REQUIRE(create_static_routing("10.0.2.0/24","10.0.1.2")==oms::rc::err_not_found);
//            REQUIRE(create_static_routing("10.0.2.0/24","10.0.1.1")==oms::rc::success);
//            SECTION("del static routing"){
//                REQUIRE(del_static_routing("10.0.2.0/24")==oms::rc::success);
//            }
//        }
//        sleep_for_next_test();
//    }

//    SECTION("set ip and del ip"){
//        REQUIRE(create_af_packet_intf("host-vppout",true)==oms::rc::success);
//        REQUIRE(set_ip("host-vpphost","10.0.1.2/24")==oms::rc::err_not_found);
//        REQUIRE(set_ip("host-vppout","10.0.1.1/24")==oms::rc::success);
//        SECTION("del ip"){
//            REQUIRE(del_ip("host-vppout","10.0.1.1/24")==oms::rc::success);
//        }
//        sleep_for_next_test();
//    }
//    SECTION("set ip and add routing"){
//        using namespace routing;
//        REQUIRE(create_af_packet_intf("host-vppout",true)==oms::rc::success);
//        REQUIRE(set_ip("host-vppout","10.0.1.1/24")==oms::rc::success);
//        REQUIRE(create_static_routing("10.0.2.0/24","10.0.1.2")==oms::rc::err_not_found);
//        REQUIRE(create_static_routing("10.0.2.0/24","10.0.1.1")==oms::rc::success);
//        SECTION("del static routing"){
//            REQUIRE(del_static_routing("10.0.2.0/24")==oms::rc::success);
//        }
//        sleep_for_next_test();
//    }


}