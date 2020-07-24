#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include "sc_plugins.h"
#include "sys_util.h"

namespace parser{
    void parse_interface_ipv46_address(sr_val_t *val,std::string &addr,uint8_t &prefix);
    void parse_nexthop_addr(sr_val_t *val,string &next_hop);
}


#endif