//
// Created by Stack on 7/25/20.
//

#ifndef SWEETCOMB_COMMON_HPP
#define SWEETCOMB_COMMON_HPP

#include <string>
#include <utility>

namespace oms {
    enum rc {
        err_failed = -1,
        err_not_found = -2,
        success = 0,
        err_invalid_arg = -4,
        err_existing = -3,
    };

    int split_ip_addr(const std::string &ip_with_prefix, std::string &ip, int &prefix_len);

    bool valid_ip(const std::string &ip);
}

#endif //SWEETCOMB_COMMON_HPP
