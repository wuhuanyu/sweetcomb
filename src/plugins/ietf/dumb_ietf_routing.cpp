#include <string>
#include <memory>

#include "sc_plugins.h"
#include "sys_util.h"
#include "utils/utils.hpp"
#include "cmds/intfcmd.h"
#include "utils/intfutils.hpp"
#include "parser.hpp"

// const char *static_routing_xpath =
//     "/ietf-routing:routing/control-plane-protocols/control-plane-protocol/"
//     "static-routes/ietf-ipv4-unicast-routing:ipv4/route";
extern const char *static_routing_xpath;

//dumb static routing callback,for test only
static int dumb_static_routing_create_cb(sr_session_ctx_t *session,
                                         const char *xpath, sr_notif_event_t event,
                                         void *private_ctx) {
    UNUSED (private_ctx);
    sr_change_iter_t *iter = nullptr;
    sr_change_oper_t op = SR_OP_CREATED;
    sr_val_t *old_val = nullptr;
    sr_val_t *new_val = nullptr;
    sr_xpath_ctx_t xpath_ctx;
    bool create = false;
    bool modify = false;
    bool del = false;
    int rv = -1;

    string dst_addr;
    string next_hop;

    int rc = SR_ERR_OK;
    SRP_LOG_DBG ("callback for '%s' get called,in %s", xpath, __FUNCTION__);
    // todo delete this,for test only
    //   return rc;

    if (SR_EV_APPLY == event) {
        return SR_ERR_OK;
    }

    SRP_LOG_DBG ("'%s' modified,event=%d", xpath, event);

    // get changes iterator
    rc = sr_get_changes_iter(session, xpath, &iter);
    if (SR_ERR_OK != rc) {
        sr_free_change_iter(iter);
        SRP_LOG_ERR ("Unable to retrieve xpath change iterator for %s", xpath);
        return rc;
    }
    foreach_change (session, iter, op, old_val, new_val) {
        //    "/routing/control-plane-protocols/control-plane-protocol/static-routes/"
        //"v4ur:ipv4/route/next-hop/simple-next-hop/next-hop-address";
        SRP_LOG_DBG ("a changed detected int '%s', op=%d",
                     new_val ? new_val->xpath : old_val->xpath, op);
        // switch operation
        dst_addr = sr_xpath_key_value(new_val ? new_val->xpath : old_val->xpath,
                                      "route", "destination-prefix", &xpath_ctx);
        if (dst_addr.empty()) {
            rc = SR_ERR_OPERATION_FAILED;
            goto nothing_todo;
        } else {
            SRP_LOG_DBG ("starting to configure dst addr '%s'", dst_addr.c_str());
        }
        sr_xpath_recover(&xpath_ctx);

        try {
            switch (op) {
                case SR_OP_MODIFIED:
                    create = true;
                    del = true;
                    SRP_LOG_DBG_MSG ("operation is to modify static routing");
                    parser::parse_nexthop_addr(new_val, next_hop);
                    break;
                case SR_OP_CREATED:
                    create = true;
                    SRP_LOG_DBG_MSG ("operation is to create static routing");
                    parser::parse_nexthop_addr(new_val, next_hop);
                    break;
                case SR_OP_DELETED:
                    SRP_LOG_DBG_MSG ("operation is to delete static routing");
                    del = true;
                    break;
                default:
                    SRP_LOG_ERR_MSG ("Unsupported sysrepo action");
                    rc = SR_ERR_INVAL_ARG;
                    goto nothing_todo;
                    break;
            }
        }
        catch (std::exception &exec) {
            SRP_LOG_ERR ("Error :%s", exec.what());
        }
        sr_free_val(old_val);
        sr_free_val(new_val);
        if (del && !dst_addr.empty()) {
            rv = 0;
            if (rv != 0) {
                SRP_LOG_ERR ("Error when deleting routing dst_addr:'%s'",
                             dst_addr.c_str());
                rc = SR_ERR_OPERATION_FAILED;
                goto nothing_todo;
            } else {
                SRP_LOG_DBG ("Deleting routing dst_addr: '%s' successfully",
                             dst_addr.c_str());
            }
        }
        if (create && !dst_addr.empty() && !next_hop.empty()) {
            // todo configure static routing
            SRP_LOG_DBG ("starting to configure dst addr '%s' nexthop_addr:%s",
                         dst_addr.c_str(), next_hop.c_str());
            rv = 0;
            if (rv != 0) {
                SRP_LOG_DBG_MSG ("configure static routing err_failed");
                rc = SR_ERR_OPERATION_FAILED;
                goto nothing_todo;
            } else {
                SRP_LOG_DBG (
                        "Configure static routing for dst_addr:'%s' successfully",
                        dst_addr.c_str());
            }
        }
    }

    nothing_todo:
    sr_free_val(old_val);
    sr_free_val(new_val);
    sr_free_change_iter(iter);
    return rc;
}


int dumb_static_routing_init(sc_plugin_main_t *pm) {
    int rc = SR_ERR_OK;
    SRP_LOG_DBG_MSG("Initializing dumb ietf interface static routing");

    rc = sr_subtree_change_subscribe(pm->session,
                                     static_routing_xpath,
                                     dumb_static_routing_create_cb, nullptr, 99,
                                     SR_SUBSCR_CTX_REUSE, &pm->subscription);
    if (SR_ERR_OK != rc) {
        goto error;
    }
    SRP_LOG_DBG_MSG("Initialize dumb ietf static routing plugin successfully");
    return SR_ERR_OK;

    error:
    SRP_LOG_ERR("Error initialize dumb_static_routing plugin %d", rc);
    return rc;

}

void dumb_static_routing_exit(__attribute__ ((unused)) sc_plugin_main_t *pm) {
}

//SC_INIT_FUNCTION (dumb_static_routing_init);

//SC_EXIT_FUNCTION (dumb_static_routing_exit);