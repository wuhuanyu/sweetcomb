//
// Created by Stack on 7/25/20.
//
#include "common.hpp"
#include <string>
#include <boost/asio.hpp>

namespace oms {
    using string = std::string;

    int split_ip_addr(const std::string &ip_with_prefix, std::string &ip, int &prefix_len) {
        size_t pos = -1;
        prefix_len = -1;
        if ((pos = ip_with_prefix.find('/')) == string::npos) return err_invalid_arg;
        ip = ip_with_prefix.substr(0, pos);
        try {
            prefix_len = std::stoi(ip_with_prefix.substr(pos + 1));
        } catch (...) {}
        if (-1 == prefix_len) return err_invalid_arg;
        return success;
    }

    bool valid_ip(const std::string &ip) {
        boost::system::error_code er;
        boost::asio::ip::make_address("10.0.1.0", er);
        return (!er);
    }
}
