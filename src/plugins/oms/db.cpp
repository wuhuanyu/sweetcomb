//
// Created by Stack on 7/25/20.
//

#include "db.hpp"
#include "common.hpp"

namespace oms {
    namespace intfdb {
        int put_ip_and_intf(const ip &iip, const std::string &intfname){
            ip2intf[iip] = intfname;
            intf2ips[intfname].insert(iip);
            return success;
        }

        int find_intfname(const ip &iip, std::string &intfname){
            if (ip2intf.count(iip)) {
                intfname = ip2intf[iip];
                return success;
            }
            return err_not_found;
        }

        int find_ips(const std::string &intfname, ips &result){
            if (intf2ips.count(intfname)) {
                auto &iips = intf2ips[intfname];
                result.clear();
                result.insert(iips.begin(), iips.end());
                return success;
            }
            return err_not_found;
        }

        int delete_ip(const std::string &intfname, const ip &iip){
            if(intf2ips.count(intfname)) {
                ip2intf.erase(iip);
                intf2ips[intfname].erase(iip);
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