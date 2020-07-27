//
// Created by Stack on 7/27/20.
//

#include "../../external/catch2/catch.hpp"
#include "db.hpp"
#include "common.hpp"
#include <utility>

TEST_CASE("lib db should work properly","[db]"){
    using namespace oms;
    using namespace oms::intfdb;
    using namespace std;
    string int1="int1";
    string int2="int2";
    string int3="int3";
    ip ip1=make_pair("10.0.0.1",24);
    ip ip2=make_pair("10.0.0.2",24);
    ip ip3=make_pair("10.0.0.3",24);
    ip ip4=make_pair("10.0.0.4",24);

    SECTION("put_ip_and_intf"){
        put_ip_and_intf(ip1,int1);
        put_ip_and_intf(ip2,int1);
        ips bag;
        REQUIRE(find_ips(int1,bag)==rc::success);
        REQUIRE(bag.size()==2);
        SECTION("delete ip"){
            REQUIRE(delete_ip(int1,ip1)==rc::success);
            ips bag2;
            REQUIRE(find_ips(int1,bag2)==rc::success);
            REQUIRE(bag2.size()==1);
            for(auto& i:bag2){
                REQUIRE(i.first=="10.0.0.2");
            }
        }
        SECTION("delete interface and ip"){
            REQUIRE(delete_intf_and_ip(int1)==rc::success);
            REQUIRE(find_ips(int1,bag)==rc::err_not_found);
        }

    }

}


