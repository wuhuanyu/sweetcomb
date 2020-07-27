//
// Created by Stack on 7/24/20.
//

#ifndef SWEETCOMB_INTF_HPP
#define SWEETCOMB_INTF_HPP

#include <string>
#include "common.hpp"
#include <vom/dump_cmd.hpp>
#include <vapi/interface.api.vapi.hpp>
#include <vapi/ip.api.vapi.hpp>


namespace oms {

    namespace log {

    }
    namespace intf {
        //create afpacket interface,must pass "host-vppout"
        int create_af_packet_intf(const std::string &host_intf);

        int create_af_packet_intf(const std::string &host_intf, bool up);

        int set_interface_state(const std::string &intf_name, bool up);

        int del_af_packet_intf(const std::string &host_intf);

        int del_intf(const std::string &intfname);

        int create_loopback_intf(const std::string &loopname, bool up);

        int del_loopback_intf(const std::string &loopname);

        int set_ip(const std::string &intf, const std::string &ip_str, int prefix_len);

        int set_ip(const std::string &intf, const std::string &ip_with_prefix);

        int del_ip(const std::string &intf, const std::string &ip_str, int prefix_len);

        int del_ip(const std::string &intf, const std::string &ip_with_prefix);


        class interface_dump : public VOM::dump_cmd<vapi::Sw_interface_dump> {
        public:
            /**
             * Default Constructor - dump everything
             */
            interface_dump();

            /*
             * Constructor to dump one interface only
             */
            interface_dump(std::string interface_name);

            /**
             * Issue the command to VPP/HW
             */
            VOM::rc_t issue(VOM::connection &con);

            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

        private:
            std::string m_name; //interface name
        };

    }
};

#endif // SWEETCOMB_INTF_HPP