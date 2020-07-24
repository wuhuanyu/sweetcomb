#ifndef __INTF_STRUCTS_H__
#define __INTF_STRUCTS_H__

#include <map>
#include <vector>
#include <memory>
// TODO remove this huge import
#include <bits/stdc++.h>
#include <boost/functional/hash.hpp>
#include <string>
#include <utility>
#include "common.hpp"
#include <unordered_map>
#include <unordered_set>
#include "utils/utils.h"

namespace structs
{

using string = std::string;
class Intf
{
  string name;
  IPS ips;
  string mac;

public:
  Intf &set_name (string n);
  Intf &add_ip (IP &ip);
  Intf &set_mac (string m);
};

class IPInterfaceMapper
{
  std::unordered_map<IP, string, boost::hash<IP>> ip_to_intfname;
  std::unordered_map<string, IPS> intfname_to_ips;
  std::unordered_map<string, int> intfname_to_idx;
  std::unordered_map<int, string> idx_to_intfname;

  IP invalid_ip;
  IPS invalid_ips;
  string invalid_intf;
  IPInterfaceMapper ();

public:
  static IPInterfaceMapper instance;
  static void populate ();
  IPInterfaceMapper get_instance ();

  // reset internal data structures
  void reset ();

  // add ip to interface
  void add_ip_to_intf (string &intf, IP &ip);
  void add_ip_to_intf (string &intf, string &ip, int prefix);
  // delete ip from interface
  void del_ip_from_intf (string &intf, IP &ip);
  void del_ip_from_intf (string &intf, string &ip, int prefix);

  // check interface has ip;
  void ip_exists (string &intf, IP &ip);
  void ip_exists (string &intf, string &ip_str, int prefix);

  void add_idx_to_intfn (string &intf, int idx);
  void add_intfn_to_idx (int idx, string &intf);

  // get ips,if intf exists,return ips,else return invalid ips
  const IPS &get_ips (string &intf);
  // get interface given ip,if ip exists,return
  const string &get_intf (IP &ip);
};

// TODO
class Device
{
  int id;
  using Intf = std::string;
  std::unordered_set<Intf> intfs;

  IPInterfaceMapper ip_interface_mapper;

public:
  Device ();

  bool has_intf (Intf &intf);
  bool add_intf (Intf &intf);
};
} // namespace structs

#endif
