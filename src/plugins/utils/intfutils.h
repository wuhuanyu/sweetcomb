#ifndef __INTFUTILS_H__
#define __INTFUTILS_H__
#include "utils.h"


// check the given interface name is af packet interface
bool is_af_intf (string intf);

// extract af packet interface name from give interface name
// e.g extract vppout from host-vppout
// params intf,given interface name, res: extracted value
// return value:0 success,-1 invalid argument
int extract_intf_from_af (string intf, string &res);

// check given interface is a loopback interface
bool is_loopback_intf (string intf_name);

//get vector of ips of given interface
vec_str get_ips(string intf);

//get interface

#endif