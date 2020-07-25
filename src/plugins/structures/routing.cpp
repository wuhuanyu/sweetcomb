#include "routing.hpp"

using namespace structs;

void routing::set_route(const IP &dst, const IP &next_hop) {
    dst_to_nexthop_ip[dst] = next_hop;
}

void routing::set_route(const IP &dst, const std::string &next_hop) {
    IP ip;
    ip.first = next_hop;
    ip.second = 32;
    dst_to_nexthop_ip[dst] = ip;
}

void routing::set_route(const IP &dst, const std::string &next_intf) {
    dst_to_nexthop_intf[dst] = next_intf;
}


int routing::get_nexthop_ip(const IP &dst, IP &out) {
    if (dst_to_nexthop_ip.count(dst) == 0) return -1;
    IP &tmp = dst_to_nexthop_ip[dst];
    out.first = tmp.first;
    out.second = tmp.second;
    return 0;
}

int routing::get_nexthop_intf(const IP &dst, std::string &out) {
    if (dst_to_nexthop_intf.count(dst) == 0) return -1;
    out = dst_to_nexthop_intf[dst];
    return 0;
}

bool routing::routing_entry_exsits(const IP &dst) {
    return dst_to_nexthop_intf.count(dst) || dst_to_nexthop_ip.count(dst);
}