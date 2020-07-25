//
// Created by Stack on 7/25/20.
//

#ifndef SWEETCOMB_DB_HPP
#define SWEETCOMB_DB_HPP

#include "common.hpp"
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace oms {
    namespace intfdb {
        //ip_with_prefix to interface
        //10.0.0.1/24->vppout
        static std::unordered_map<ip,std::string,boost::hash<ip>> ip2intf;
        static std::unordered_map<std::string,ips> intf2ips;

        int put_ip_and_intf(const ip &iip, const std::string &intfname);

        int find_intfname(const ip &iip, std::string &intfname);

        int find_ips(const std::string &intfname, ips &iips);
        //delete ip from interface;
        int delete_ip(const std::string &intfname, const ip &iip);
        //delete interface with all ips;
        int delete_intf_and_ip(const std::string &intfname);


    }
}

#endif //SWEETCOMB_DB_HPP
