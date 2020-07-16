#include "intfutils.h"

bool is_af_intf (string intf)
{
  return intf.find ("host") != string::npos;
}

int extract_intf_from_af (string intf, string &res)
{
  size_t pos = intf.find ("host");
  if (pos == string::npos)
    return -1;
  vec_str splited = split (intf, '-');
  if (splited.size () != 2)
    return -1;
  res = splited[1];
  return 0;
}

bool is_loopback_intf (string intf_name)
{
  return intf_name.find ("loop");
}