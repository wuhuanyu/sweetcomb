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
    sr_error_e sr_error(int code){
        sr_error_e rv;
        switch (code) {
            case err_failed:
                rv=SR_ERR_OPERATION_FAILED;
                break;
            case err_not_found:
                rv=SR_ERR_NOT_FOUND;
                break;
            case success:
                rv=SR_ERR_OK;
                break;
            case err_invalid_arg:
                rv=SR_ERR_INVAL_ARG;
                break;
            case err_existing:
                rv=SR_ERR_DATA_EXISTS;
                break;
            default:
                rv=SR_ERR_UNSUPPORTED;
                break;
        }
        return rv;
    }
}
