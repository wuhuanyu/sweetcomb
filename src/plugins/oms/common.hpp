//
// Created by Stack on 7/25/20.
//

#ifndef SWEETCOMB_COMMON_HPP
#define SWEETCOMB_COMMON_HPP

#include <string>
#include <utility>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <sysrepo.h>
namespace oms {
    enum rc {
        err_failed = -1,
        err_not_found = -2,
        success = 0,
        err_invalid_arg = -4,
        err_existing = -3,
    };
    using ip=std::pair<std::string,int>;
    using ips=std::unordered_set<ip,boost::hash<ip>>;
#define VALID_IP_PREFIX_LEN(len) (len>0&&len<32)
    int split_ip_addr(const std::string &ip_with_prefix, std::string &ip, int &prefix_len);

    bool valid_ip(const std::string &ip);

    sr_error_e sr_err(int code);
}

#endif //SWEETCOMB_COMMON_HPP
