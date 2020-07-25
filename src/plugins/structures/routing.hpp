#ifndef __STRUCT_ROUTING_HPP__
#define __STRUCT_ROUTING_HPP__

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include "common.hpp"


namespace structs {
    //support unicast ipv4 routing only,for now
    class routing {
        static routing instance;
        std::unordered_map<IP, IP, boost::hash<IP>> dst_to_nexthop_ip;
        std::unordered_map<IP, std::string, boost::hash<IP>> dst_to_nexthop_intf;


        routing();

    public:
        routing get_instance();

        void set_route(const IP &dst, const IP &next_hop);

        void set_route(const IP &dst, const std::string &next_hop);

        void set_route(const IP &dst, const IP &next_inf);

        //if no such routing entry exists,return -1,else return 0;
        int get_nexthop_ip(const IP &dst, IP &out);

        //if no such routing entry exists,return -1,else return 0;
        int get_nexthop_intf(const IP &dst, std::string &out);

        bool routing_entry_exsits(const IP &dst);

    };

} // namespace structs



#endif