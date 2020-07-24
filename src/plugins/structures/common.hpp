#ifndef __STRUCTS_COMMON_HPP__
#define __STRUCTS_COMMON_HPP__
#include <utility>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace structs
{
using IP = std::pair<std::string, int>;
using IPS = std::unordered_set<IP, boost::hash<IP>>;
} // namespace structs


#endif