#ifndef __UTILS_H__
#define __UTILS_H__

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

using string=std::string;
using vec_str=std::vector<string>; 

//concat vector of strings to a single string,seperated by c
string concat_to_str(vec_str &strs,char c);

//concat vector of cmds
string concat_cmds(vec_str &cmds);

//split string to a vector of string
vec_str split(string s,char token);

//run commands
int run_command(string &command);

//run commands
int run_command(const char *cmd);



#endif
