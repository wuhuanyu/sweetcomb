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
#include "common.hpp"
#include "../utils/intfutils.hpp"
#include "db.hpp"

#include <vom/dump_cmd.hpp>
#include <vapi/interface.api.vapi.hpp>
#include <vapi/ip.api.vapi.hpp>

namespace oms {
    namespace intf {
        using namespace VOM;
        using interface = VOM::interface;
        using OM = VOM::OM;
        using rc_t = VOM::rc_t;
        using admin_state_t = interface::admin_state_t;
        using l3_binding = VOM::l3_binding;
        using oms::rc;

        int create_af_packet_intf(const std::string &host_intf, bool up) {
            std::string intf_name;
            if(extract_intf_name(host_intf,intf_name)!=rc::success) return err_invalid_arg;
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
            std::string intf_name_;
            if(extract_intf_name(intf_name,intf_name_)!=success) return err_invalid_arg;

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
            std::string intf_name;
            if(extract_intf_name(host_intf,intf_name)!=success) return err_invalid_arg;

            auto intf = interface::find(intf_name);
            if (!intf) {
                return err_not_found;
            }
            OM::mark(intf_name);
            OM::sweep(intf_name);
            //todo how to handle failure?
            intfdb::delete_intf_and_ip(intf_name);
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
            return success;
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

        int set_ip(const string &intf_name, const string &ip_str, const int prefix_len) {
            if (!VALID_IP_PREFIX_LEN(prefix_len)) return err_invalid_arg;
            std::string intf_name_;
            if(extract_intf_name(intf_name,intf_name_)!=success) return err_invalid_arg;


            std::shared_ptr<interface> i = interface::find(intf_name_);
            if (!i) {
                return rc::err_not_found;
            }
            VOM::route::prefix_t addr(ip_str, prefix_len);
            std::shared_ptr<l3_binding> ip_binding =
                    std::make_shared<l3_binding>(*i, addr);

#define KEY(l3) "l3_" + l3->itf().name() + "_" + l3->prefix().to_string()
            if (OM::write(KEY(ip_binding), *ip_binding) != rc_t::OK) {
#undef KEY
                return err_failed;
            }
            ip ip_with_prefix=std::make_pair(ip_str, prefix_len);

            intfdb::put_ip_and_intf(ip_with_prefix, intf_name);
            return rc::success;
        }

        int set_ip(const std::string &intf, const string &ip_with_prefix) {
            string ip;
            int prefix_len=-1;
            if(split_ip_addr(ip_with_prefix,ip,prefix_len)!=success) return err_invalid_arg;
            return set_ip(intf, ip, prefix_len);
        }

        int del_ip(const std::string &intf, const std::string &ip_str, const int prefix_len) {
            string intfname;

            if(extract_intf_name(intf,intfname)!=success) return err_invalid_arg;
            if(!valid_ip(ip_str)) return err_invalid_arg;
            if (!VALID_IP_PREFIX_LEN(prefix_len)) return err_invalid_arg;

            auto i = interface::find(intfname);
            if (!i) return err_not_found;
            VOM::route::prefix_t addr(ip_str, prefix_len);

            std::shared_ptr<l3_binding> ip_binding =
                    std::make_shared<l3_binding>(*i, addr);

#define KEY(l3) "l3_" + l3->itf().name() + "_" + l3->prefix().to_string()
            OM::mark(KEY(ip_binding));
            OM::sweep(KEY(ip_binding));
#undef KEY
            ip iip=std::make_pair(ip_str,prefix_len);
            //todo how to handle failure
            intfdb::delete_ip(intfname,iip);
            return success;
        }

        int del_ip(const std::string &intf, const string &ip_with_prefix) {

            string ip_str;
            int prefix_len=-1;
            if(split_ip_addr(ip_with_prefix,ip_str,prefix_len)!=success){
                return err_invalid_arg;
            }
            return del_ip(intf, ip_str, prefix_len);
        }

        int del_intf(const std::string &intf_name){
            string intfname;
            if(extract_intf_name(intf_name,intfname)!=success){
                return err_invalid_arg;
            }
            auto intf = interface::find(intfname);
            if (!intf) {
                return err_not_found;
            }
            OM::mark(intfname);
            OM::sweep(intfname);
            //todo how to handle failure?
            intfdb::delete_intf_and_ip(intfname);
            return success;

        }

        interface_dump::interface_dump() {
        }

        interface_dump::interface_dump(std::string interface_name)
                : m_name(interface_name) {
        }

        rc_t
        interface_dump::issue(connection &con) {
            m_dump.reset(new msg_t(con.ctx(), std::ref(*this)));

            auto &payload = m_dump->get_request().get_payload();

            if (m_name.empty()) {
                payload.name_filter_valid = 0;
            } else {
                payload.name_filter_valid = 1;
                strcpy((char *) payload.name_filter.buf, m_name.c_str());
            }

            VAPI_CALL(m_dump->execute());

            wait();

            return rc_t::OK;
        }

        std::string
        interface_dump::to_string() const {
            return ("itf-dump");
        }

    } // namespace intf
} // namespace oms



