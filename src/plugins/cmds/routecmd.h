#include "utils/utils.h"

namespace cmd
{
    //add static route
    int add_static_route(const string &dst_subnet,int dst_prefix,const string &next_hop_addr);

    //add "10.0.0.0/24" via "10.10.2.0"
    int add_static_route(const string &dst,const string &next_hop_addr);
    
    int del_static_route(const string &dst_subnet,int dst_prefix);
    int del_static_route(const string &dst);

} // namespace cmd


