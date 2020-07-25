//
// Created by Stack on 7/24/20.
//

#include "intf.hpp"
#include <string>
#include <vom/hw.hpp>
#include <vom/om.hpp>
#include <vom/types.hpp>
#include <vom/prefix.hpp>
#include <vom/interface_cmds.hpp>
#include <vom/tap_interface.hpp>
#include <memory>
#include <vom/l3_binding.hpp>
#include "utils/intfutils.h"
#include <iostream>
#include "db.hpp"

namespace oms {
    namespace intf {
#define VALID_IP_PREFIX_LEN(len) (len>0&&len<32)
        using interface = VOM::interface;
        using OM = VOM::OM;
        using rc_t = VOM::rc_t;
        using admin_state_t = interface::admin_state_t;
        using l3_binding = VOM::l3_binding;

        int create_af_packet_intf(const std::string &host_intf, bool up) {
            std::string intf_name = extract_intf_name(host_intf);
            if (intf_name.empty()) return err_invalid_arg;
            interface::admin_state_t state =
                    (up ? interface::admin_state_t::UP : interface::admin_state_t::DOWN);
            std::shared_ptr<interface> intf = std::make_shared<interface>(
                    intf_name, interface::type_t::AFPACKET, state);
            if (OM::write(intf->name(), *intf) != VOM::rc_t::OK) {
                return rc::err_failed;
            }
            return rc::success;
        }

        int create_af_packet_intf(const std::string &host_intf) {
            return create_af_packet_intf(host_intf, false);
        }

        int set_interface_state(const std::string &intf_name, bool up) {
            string intf_name_ = extract_intf_name(intf_name);
            if (intf_name.empty()) return err_invalid_arg;

            auto intf = interface::find(intf_name_);
            if (!intf) {
                return rc::err_not_found;
            }
            admin_state_t state = (up ? admin_state_t::UP : admin_state_t::DOWN);
            intf->set(state);
            if (OM::write(intf->name(), *intf) != rc_t::OK) {
                return rc::err_failed;
            }
            return rc::success;
        }

        int del_af_packet_intf(const std::string &host_intf) {
            string intfname = extract_intf_name(host_intf);
            if (intfname.empty()) return err_invalid_arg;

            auto intf = interface::find(intfname);
            if (!intf) {
                return err_not_found;
            }
            OM::mark(host_intf);
            OM::sweep(host_intf);
            //todo how to handle failure?
            intfdb::delete_intf_and_ip(intfname);
            return success;
        }

        int create_loopback_intf(const std::string &loopname, bool up) {
            auto intf = interface::find(loopname);
            if (intf) {
                return err_existing;
            }
            admin_state_t state = (up ? admin_state_t::UP : admin_state_t::DOWN);
            std::shared_ptr<interface> loopintf = std::make_shared<interface>(
                    loopname, interface::type_t::LOOPBACK, state);
            if (OM::write(loopname, *loopintf) != rc_t::OK) {
                return err_failed;
            }
            return err_existing;
        }

        int del_loopback_intf(const std::string &loopname) {
            auto intf = interface::find(loopname);
            if (intf) {
                return err_not_found;
            }
            OM::mark(loopname);
            OM::sweep(loopname);
            //todo how to handle failure;
            intfdb::delete_intf_and_ip(loopname);
            return success;
        }

        int set_ip(const string &intf_name, const string &ip, const int prefix_len) {
            if (!VALID_IP_PREFIX_LEN(prefix_len)) return err_invalid_arg;

            string intfname = extract_intf_name(intf_name);
            if (intf_name.empty()) return err_invalid_arg;

            std::shared_ptr<interface> i = interface::find(intf_name);
            if (!i) {
                return rc::err_not_found;
            }
            VOM::route::prefix_t addr(ip, prefix_len);
            std::shared_ptr<l3_binding> ip_binding =
                    std::make_shared<l3_binding>(*i, addr);

#define KEY(l3) "l3_" + l3->itf().name() + "_" + l3->prefix().to_string()
            if (OM::write(KEY(ip_binding), *ip_binding) != rc_t::OK) {
#undef KEY
                return err_failed;
            }
            string ip_with_prefix = ip + "/" + std::to_string(prefix_len);

            intfdb::put_ip_and_intf(ip_with_prefix, intf_name);
            return rc::success;
        }

        int set_ip(const std::string &intf, const string &ip_with_prefix) {
            size_t pos = string::npos;
            if ((pos = ip_with_prefix.find('/')) == string::npos) return err_invalid_arg;
            string ip = ip_with_prefix.substr(pos);
            int prefix_len = -1;
            try {
                prefix_len = std::stoi(ip_with_prefix.substr(pos + 1));
            } catch (...) {

            }
            if (-1 == prefix_len) return err_invalid_arg;
            return set_ip(intf, ip, prefix_len);
        }

        int del_ip(const std::string &intf, const std::string &ip, const int prefix_len) {
            string intfname = extract_intf_name(intf);
            if (!VALID_IP_PREFIX_LEN(prefix_len)) return err_invalid_arg;
            auto i = interface::find(intfname);
            if (!i) return err_not_found;
            VOM::route::prefix_t addr(ip, prefix_len);

            std::shared_ptr<l3_binding> ip_binding =
                    std::make_shared<l3_binding>(*i, addr);

#define KEY(l3) "l3_" + l3->itf().name() + "_" + l3->prefix().to_string()
            OM::mark(KEY(ip_binding));
            OM::sweep(KEY(ip_binding));
#undef KEY
            string ip_with_prefix=ip+"/"+std::to_string(prefix_len);
            //todo how to handle failure
            intfdb::delete_ip(intfname,ip_with_prefix);
            return success;
        }

        int del_ip(const std::string &intf, const string &ip_with_prefix) {
            size_t pos = string::npos;
            if ((pos = ip_with_prefix.find('/')) == string::npos) return err_invalid_arg;
            string ip = ip_with_prefix.substr(pos);
            int prefix_len = -1;
            try {
                prefix_len = std::stoi(ip_with_prefix.substr(pos + 1));
            } catch (...) {

            }
            if (-1 == prefix_len) return err_invalid_arg;
            return del_ip(intf, ip, prefix_len);
        }

    } // namespace intf
} // namespace oms