#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "utils.h"

using string = std::string;
using vec_str = std::vector<string>;

string concat_to_str (const vec_str &strs, char c)
{
  std::stringstream ss;
  for (int i = 0; i < strs.size (); i++)
    {
      ss << strs[i];
      if (i == strs.size () - 1)
        continue;
      ss << c;
    }
  return ss.str ();
}

string concat_cmds (const vec_str &cmds)
{
  return concat_to_str (cmds, ' ');
}

vec_str split (string s, char token)
{
  vec_str res;
  size_t pos;
  string part;
  while ((pos = s.find (token)) != string::npos)
    {
      res.push_back (s.substr (0, pos));
      s = s.erase (0, pos + 1);
    }
  res.push_back (s);
  return res;
}

int run_command (string &command)
{
  return std::system (command.c_str ());
}

int run_command (const char *cmd)
{
  return std::system (cmd);
}

int concat_ip(const string &dst,int prefix,string &res)
{
  if(prefix>=32||prefix<=0) return -1;
  res=dst+"/"+std::to_string(prefix);
  return 0;
}

int run_command(const vec_str &cmds)
{
  string cmd=concat_cmds(cmds);
  return run_command(cmd);
}



