//
// Created by Stack on 7/25/20.
//

#include "db.hpp"
#include "common.hpp"

namespace oms {
    namespace intfdb {
        int put_ip_and_intf(const std::string &ip_with_prefix, const std::string &intfname) {
            ip2intf[ip_with_prefix] = intfname;
            intf2ips[intfname].insert(ip_with_prefix);
            return success;
        }

        int find_intfname(const std::string &ip_with_prefix, std::string &intfname) {
            if (ip2intf.count(ip_with_prefix)) {
                intfname = ip2intf[ip_with_prefix];
                return success;
            }
            return err_not_found;
        }

        int find_ips(const std::string &intfname, std::unordered_set<std::string> &result) {
            if (intf2ips.count(intfname)) {
                auto &ips = intf2ips[intfname];
                ips.insert(ips.begin(), ips.end());
                return success;
            }
            return err_not_found;
        }

        int delete_ip(const std::string &intfname, const std::string &ip_with_prefix){
            if(intf2ips.count(intfname)) {
                ip2intf.erase(ip_with_prefix);
                intf2ips[intfname].erase(ip_with_prefix);
                return success;
            }
            return err_not_found;
        }

        int delete_intf_and_ip(const std::string &intfname){
            //find ip
            if(intf2ips.count(intfname)){
                auto &ips=intf2ips[intfname];
                for(auto &ip:ips) ip2intf.erase(ip);
                intf2ips.erase(intfname);
                return success;
            }
            return err_not_found;
        }
    }
}