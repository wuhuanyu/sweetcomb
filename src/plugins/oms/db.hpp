//
// Created by Stack on 7/25/20.
//

#ifndef SWEETCOMB_DB_HPP
#define SWEETCOMB_DB_HPP

#include "common.hpp"
#include <unordered_map>
#include <string>
#include <unordered_set>

namespace oms {
    namespace intfdb {
        //ip_with_prefix to interface
        //10.0.0.1/24->vppout
        static std::unordered_map<std::string, std::string> ip2intf;
        // vppout->{10.0.0.1/24,10.0.3.1/24}
        static std::unordered_map<std::string, std::unordered_set<std::string>> intf2ips;

        int put_ip_and_intf(const std::string &ip_with_prefix, const std::string &intfname);

        int find_intfname(const std::string &ip_with_prefix, std::string &intfname);

        int find_ips(const std::string &intfname, std::unordered_set<std::string> &ips);
        //delete ip from interface;
        int delete_ip(const std::string &intfname, const std::string &ip_with_prefix);
        //delete interface with all ips;
        int delete_intf_and_ip(const std::string &intfname);


    }
}

#endif //SWEETCOMB_DB_HPP
