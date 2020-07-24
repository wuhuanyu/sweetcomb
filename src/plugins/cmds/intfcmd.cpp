#include "intfcmd.h"
#include "utils/utils.h"

int cmd::add_intf_ip(const string &intf,const string &ip){
    vec_str cmds={
        "set int ip address",
        intf,
        ip
    };
    string cmd=concat_cmds(cmds);
    return run_vppctl_command(cmd);
}

int cmd::add_intf_ip(const string &intf,const string &ip,int prefix_length){
    string ip_=ip+"/"+std::to_string(prefix_length);
    return add_intf_ip(intf,ip_);
}


int cmd::del_intf_ip(const string &intf,const string&ip){
    vec_str cmds={
        "set int ip address del",
        intf,
        ip
    };
    string cmd=concat_cmds(cmds);
    return run_vppctl_command(cmd);
}

int cmd::del_intf_ip(const string &intf,const string &ip,int prefix_length){
    string ip_=ip+"/"+std::to_string(prefix_length);
    return del_intf_ip(intf,ip_);
}

int cmd::del_intf_all_ip(const string &intf){
    vec_str cmds={
        "set int ip address",
        intf,
        "all"
    };
    string cmd=concat_cmds(cmds);
    return run_vppctl_command(cmd);
}

int cmd::create_af_intf (string af_intf_name)
{
  vec_str cmds={
      "create host-interface name",af_intf_name
  };
  return run_vppctl_command(cmds);
}

int cmd::set_intf_status (string intf_name, bool enable)
{

    vec_str cmds={
        "set int state",intf_name,(enable?"up":"down")
    };
    return run_vppctl_command(cmds);
}

