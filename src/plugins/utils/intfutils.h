#ifndef __INTFUTILS_H__
#define __INTFUTILS_H__

#include "utils.h"


// check the given interface name is af packet interface
bool is_af_intf(const string &intf);

//return extracted_intf_name
// if host-vppout return vppout
//else return loop0
int extract_intf_name(const string& intf,string &result);



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