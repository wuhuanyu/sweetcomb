#ifndef __OPER_INTERFACE_H_
#define __OPER_INTERFACE_H_

#include <vom/dump_cmd.hpp>
#include <vapi/interface.api.vapi.hpp>
#include <vapi/ip.api.vapi.hpp>

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

class ip_address_dump : public VOM::dump_cmd<vapi::Ip_address_dump> {

};

class ip_dump : public VOM::dump_cmd<vapi::Ip_dump> {

};


#endif //__OPER_INTERFACE_H_
