#include "intfcmd.h"
#include "utils.h"

int add_intf_ip(const string &intf,const string &ip){
    vec_str cmds={
        "set int ip address",
        intf,
        ip
    };
    string cmd=concat_cmds(cmds);
    run_command(cmd);
}


int del_intf_ip(const string &intf,const string&ip){
    vec_str cmds={
        "set int ip address del",
        intf,
        ip
    };
    string cmd=concat_cmds(cmds);
    run_command(cmd);
}

int del_intf_all_ip(const string &intf){
    vec_str cmds={
        "set int ip address",
        intf,
        "all"
    };
    string cmd=concat_cmds(cmds);
    run_command(cmd);
}

int create_af_intf (string af_intf_name)
{
  string command = "vppctl create host-interface name " + af_intf_name;
  return run_command (command);
}

int set_intf_status (string intf_name, bool enable)
{
  string command =
      "vppctl set int state " + intf_name + " " + (enable ? "up" : "down");
  return run_command (command);
}

