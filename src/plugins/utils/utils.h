#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#define DEFAULT_SOCK "/run/vpp/cli.sock"

using string = std::string;
using vec_str = std::vector<string>;

//concat vector of strings to a single string,seperated by c
string concat_to_str(const vec_str &strs, char c);

//concat vector of cmds
string concat_cmds(const vec_str &cmds);

//split string to a vector of string
vec_str split(string s, char token);

//run commands
int run_command(string &command);

//run vppctl commands
int run_command(const char *cmd);

int run_command(const vec_str &cmds);

int run_vppctl_command(const char *cmd);

int run_vppctl_command(const vec_str &cmds);

int run_vppctl_command(const string &cmd);

int concat_ip(const string &dst, int prefix, string &res);

// int get_env(const char *env);
// int get_env(const string &env);

#endif
