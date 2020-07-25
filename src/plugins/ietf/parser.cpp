#include "parser.hpp"

void parser::parse_interface_ipv46_address(sr_val_t *val, std::string &addr, uint8_t &prefix) {
    if (val == nullptr)
        throw std::runtime_error("Null pointer");

    if (SR_LIST_T == val->type) {
        /* create on list item - reset state vars */
        addr.clear();
    } else {
        if (sr_xpath_node_name_eq(val->xpath, "ip")) {
            addr = val->data.string_val;
        } else if (sr_xpath_node_name_eq(val->xpath, "prefix-length")) {
            prefix = val->data.uint8_val;
        } else if (sr_xpath_node_name_eq(val->xpath, "netmask")) {
            prefix = utils::netmask_to_plen(
                    boost::asio::ip::address::from_string(val->data.string_val));
        }
    }
}

void parser::parse_nexthop_addr(sr_val_t *val, std::string &next_hop) {
    if (val == nullptr) {
        throw std::runtime_error("sr_val_t null pointer");
    }
    if (SR_LIST_T == val->type) {
        next_hop.clear();
    } else {
        if (sr_xpath_node_name_eq(val->xpath, "next-hop-address")) {
            next_hop = val->data.string_val;
        }
    }
}