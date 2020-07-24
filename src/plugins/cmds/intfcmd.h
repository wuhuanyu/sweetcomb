#ifndef __INTFCMD_H__
#define __INTFCMD_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using string=std::string;

namespace  cmd
{
// add ip to interface,ip with netmask
int add_intf_ip (const std::string &intf, const std::string &ip);
int add_intf_ip (const std::string &intf, const std::string &ip,
                 int prefix_length);

// del ip from certain interface,ip with netmask
int del_intf_ip (const std::string &intf, const std::string &ip);
int del_intf_ip (const std::string &intf, const std::string &ip,
                 int prefix_length);

// del all ips from certain interface
int del_intf_all_ip (const std::string &intf);

// create linux af packet interface with no "host-" prefix
int create_af_intf (string af_intf_name);

// set interface status ,up or down
int set_intf_status (string intf_name, bool up);

// delete interface
// todo delete interface
int del_intf (string &intf_name);
} // namespace  cmd






#endif
