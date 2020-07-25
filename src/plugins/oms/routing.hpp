//
// Created by Stack on 7/24/20.
//

#ifndef SWEETCOMB_ROUTING_HPP
#define SWEETCOMB_ROUTING_HPP
#include <string>
#include "common.hpp"
#include "../common.hpp"

namespace oms {
    namespace routing {
        int create_static_routing(const std::string &dst_net, const std::string &next_hop_addr);

        int del_static_routing(const std::string &dst_addr);

        int create_static_routing(const std::string &dst_net, const std::string &next_hop_addr,
                                  const std::string &intf);

    }
}

#endif // SWEETCOMB_ROUTING_HPP
