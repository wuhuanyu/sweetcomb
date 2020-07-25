#ifndef __INTFUTILS_H__
#define __INTFUTILS_H__

#include "utils.h"


// check the given interface name is af packet interface
bool is_af_intf(string intf);

//return extracted_intf_name
// if host-vppout return vppout
//else return loop0
string extract_intf_name(string intf);

// extract af packet interface name from give interface name
// e.g extract vppout from host-vppout
// params intf,given interface name, res: extracted value
// return value:0 success,-1 invalid argument
int extract_intf_from_af(string intf, string &res);

// check given interface is a loopback interface
bool is_loopback_intf(string intf_name);

//get vector of ips of given interface
vec_str get_ips(string intf);

//get interface

namespace utils {
    namespace intf {
        vec_str split_ip(const string &ip_with_prefix);

        inline bool is_valid_prefix_len(int prefix_len);
    }
}

#endif