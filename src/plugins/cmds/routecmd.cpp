#include "routecmd.h"
#include "utils/utils.h"
#include <iostream>
int cmd::add_static_route (const string &dst_subnet, int dst_prefix,
                           const string &next_hop_addr)
{

  string dst;
  if (concat_ip (dst_subnet, dst_prefix, dst) != 0)
    return -1;
  vec_str cmds = {"ip route add", dst, "via", next_hop_addr};
  return run_vppctl_command (cmds);
}

int cmd::del_static_route (const string &dst_subnet, int dst_prefix)
{
  string dst;
  if (concat_ip (dst_subnet, dst_prefix, dst) != 0)
    return -1;
  vec_str cmds = {"ip route del", dst};
  return run_vppctl_command (cmds);
}
int cmd::del_static_route (const string &dst)
{
  vec_str cmds = {"ip route del", dst};
  return run_vppctl_command (cmds);
}

int cmd::add_static_route (const string &dst, const string &next_hop_addr)
{
  vec_str cmds = {"ip route add", dst, "via", next_hop_addr};
  return run_vppctl_command (cmds);
}
