#include "interface.hpp"
#include <utility>

using namespace structs;

IPInterfaceMapper::IPInterfaceMapper() {
    invalid_intf = "";
    invalid_ip = std::make_pair<string, int>("invalid_ip", -1);
    // invalid_ips.emplace(invalid_ip);
}

void IPInterfaceMapper::reset() {
    ip_to_intfname = std::unordered_map<IP, string, boost::hash<IP>>();
    intfname_to_ips = std::unordered_map<string, IPS>();
}

void IPInterfaceMapper::add_ip_to_intf(string &intf, IP &ip) {
    intfname_to_ips[intf].emplace(ip.first, ip.second);
}

void IPInterfaceMapper::add_ip_to_intf(string &intf, string &ip_str,
                                       int prefix) {
    intfname_to_ips[intf].emplace(ip_str, prefix);
}

void IPInterfaceMapper::del_ip_from_intf(string &intf, IP &ip) {
    intfname_to_ips[intf].erase(ip);
}

void IPInterfaceMapper::del_ip_from_intf(string &intf, string &ip_str,
                                         int prefix) {
    // IP ip=std::make_pair<string&,int>(ip_str,prefix);
    IP ip;
    ip.first = ip_str;
    ip.second = prefix;
    del_ip_from_intf(intf, ip);
}

void IPInterfaceMapper::add_idx_to_intfn(string &intf, int idx) {
    intfname_to_idx[intf] = idx;
}

void IPInterfaceMapper::add_intfn_to_idx(int idx, string &intfn) {
    idx_to_intfname[idx] = intfn;
}

const IPS &IPInterfaceMapper::get_ips(string &intf) {
    if (intfname_to_ips.count(intf))
        return intfname_to_ips[intf];
    return invalid_ips;
}

const string &IPInterfaceMapper::get_intf(IP &ip) {
    if (ip_to_intfname.count(ip))
        return ip_to_intfname[ip];
    return invalid_intf;
}

void IPInterfaceMapper::populate() {
}

Intf &Intf::set_name(string n) {
    name = n;
    return *this;
}

Intf &Intf::add_ip(IP &ip) {
    ips.emplace(ip.first, ip.second);
    return *this;
}

Intf &Intf::set_mac(string m) {
    mac = m;
    return *this;
}