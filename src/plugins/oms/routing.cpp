//
// Created by Stack on 7/24/20.
//

#include "routing.hpp"
#include <string>
#include <memory>
#include "utils/intfutils.h"
#include <vom/interface.hpp>
#include "common.hpp"
#include <vom/route.hpp>
#include <boost/asio.hpp>

namespace oms {
    namespace routing {
        using namespace std;
        using interface = VOM::interface;
        using namespace VOM::route;

        int create_static_routing(const std::string &dst_addr, const std::string &next_hop_addr) {
            //find
        }

        int del_static_routing(const std::string &dst_addr) {
            string key = string("route_") + "target_" + dst_addr;
            VOM::OM::mark(key);
            VOM::OM::sweep(key);
            return success;
        }

        int create_static_routing(const std::string &dst_net, const std::string &next_hop_addr,
                                  const std::string &intf_) {
            string intfname = extract_intf_name(intf_);
            if (intfname.empty()) return err_invalid_arg;

            auto intf = interface::find(intfname);
            if (!intf) return err_not_found;
            string dst_subnet_ip;
            int dst_subnet_prefix;
            if (split_ip_addr(dst_net, dst_subnet_ip, dst_subnet_prefix) != success) {
                return err_invalid_arg;
            }
            if (!valid_ip(dst_subnet_ip)) return err_invalid_arg;
            if(!valid_ip(next_hop_addr)) return err_invalid_arg;

            VOM::route::prefix_t target(dst_subnet_ip, dst_subnet_prefix);
            auto next_hop = boost::asio::ip::make_address(next_hop_addr);
            VOM::route::path p(next_hop, *intf);
            std::shared_ptr<ip_route> r = std::make_shared<ip_route>(target, p);

            string key = string("route_") + "target_" + dst_net;
            if (VOM::OM::write(key, *r) != VOM::rc_t::OK) return err_failed;
            return success;
        }


    }
}
