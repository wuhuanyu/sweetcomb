#ifndef __INTFCMD_H__
#define __INTFCMD_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using string=std::string;

//add ip to interface,ip with netmask
int add_intf_ip(const std::string &intf,const std::string &ip);

//del ip from certain interface,ip with netmask
int del_intf_ip(const std::string &intf,const std::string &ip);

//del all ips from certain interface
int del_intf_all_ip(const std::string &intf);

// create linux af packet interface with no "host-" prefix
int create_af_intf(string af_intf_name);

// set interface status ,up or down
int set_intf_status(string intf_name,bool enable);



#endif
