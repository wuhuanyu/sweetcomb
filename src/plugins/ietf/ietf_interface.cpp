/*
 * Copyright (c) 2016 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This file implements:
 *  - creation, configuration, deletion of an interface.
 *  - adding, modifying, removing a prefix from an interface
 *  - streaming of an interface statistics
 *
 * Even physical interfaces must be created before applying configuration to
 * them. Else, it results in undefined behavior. (cf RFC 8343)
 */

#include <string>
#include <exception>
#include <memory>

#include <vom/interface.hpp>
#include <vom/om.hpp>
#include <vom/l3_binding.hpp>
#include <vom/route.hpp>

#include <vpp-oper/interface.hpp>

#include "sc_plugins.h"
#include "sys_util.h"
#include "utils/utils.h"
#include "cmds/intfcmd.h"
#include "utils/intfutils.h"
#include "structures/intf_structs.hpp"
using namespace std;

using VOM::HW;
using VOM::interface;
using VOM::l3_binding;
using VOM::OM;
using VOM::rc_t;

using type_t = VOM::interface::type_t;
using admin_state_t = VOM::interface::admin_state_t;

/* @brief creation of ethernet devices */
static int ietf_interface_create_cb (sr_session_ctx_t *session,
                                     const char *xpath, sr_notif_event_t event,
                                     void *private_ctx)
{
  UNUSED (private_ctx);
  string if_name;
  bool enabled = false;
  sr_change_iter_t *iter = nullptr;
  sr_xpath_ctx_t xpath_ctx;
  sr_val_t *old_val = nullptr;
  sr_val_t *new_val = nullptr;
  sr_change_oper_t op;
  bool create = false, remove = false, modify = false;
  int rc;

  SRP_LOG_INF ("In %s", __FUNCTION__);

  /* no-op for apply, we only care about SR_EV_{ENABLED,VERIFY,ABORT} */
  if (SR_EV_VERIFY != event)
    return SR_ERR_OK;

  SRP_LOG_DBG ("'%s' modified, event=%d", xpath, event);

  /* get changes iterator */

  rc = sr_get_changes_iter (session, xpath, &iter);
  if (SR_ERR_OK != rc)
    {
      sr_free_change_iter (iter);
      SRP_LOG_ERR ("Unable to retrieve change iterator: %s", sr_strerror (rc));
      return SR_ERR_OPERATION_FAILED;
    }

  SRP_LOG_DBG_MSG ("going into foreach_change ");
  // for loop to strip new values,given new xpath and new path;
  foreach_change (session, iter, op, old_val, new_val)
  {
    if (old_val)
      {
        SRP_LOG_DBG ("Old val xpath:%s", old_val->xpath);
      }
    if(new_val)
      {
        SRP_LOG_DBG ("new value xpath:%s", new_val->xpath);
      }
    SRP_LOG_INF ("Change xpath: %s",
                 old_val ? old_val->xpath : new_val->xpath);
    switch (op)
      {
      case SR_OP_MODIFIED:
        SRP_LOG_INF_MSG ("operation is to modify");
        if (sr_xpath_node_name_eq (new_val->xpath, "enabled"))
          {
            // strip interface name from xpath
            if_name = sr_xpath_key_value (new_val->xpath, "interface", "name",
                                          &xpath_ctx);

            SRP_LOG_DBG ("set interface state %s",
                         (new_val->data.bool_val ? "enabled" : "disabled"));
            enabled=new_val->data.bool_val;
            modify = true;
          }
        break;
      case SR_OP_CREATED:
        SRP_LOG_DBG_MSG ("operation is to create");
        // compared string after the last slash in xpath(node name) with
        // provided string
        // if leaf=="name" then set name
        if (sr_xpath_node_name_eq (new_val->xpath, "name"))
          {
            SRP_LOG_DBG ("builder set name:%s", new_val->data.string_val);
            if_name = new_val->data.string_val;
            create = true;
          }
        else if (sr_xpath_node_name_eq (new_val->xpath, "type"))
          {
            // todo add type
            SRP_LOG_DBG ("builder set type:%s", new_val->data.string_val);
          }
        else if (sr_xpath_node_name_eq (new_val->xpath, "enabled"))
          {
            // if leaf=="enabled"
            // set state
            SRP_LOG_DBG ("set state %s:",
                         new_val->data.bool_val ? "enabled" : "disabled");
            enabled = new_val->data.bool_val;
          }
        break;
      case SR_OP_DELETED:
        SRP_LOG_DBG_MSG ("operation is to delete");
        if (sr_xpath_node_name_eq (old_val->xpath, "name"))
          {
            if_name = old_val->data.string_val;
            SRP_LOG_DBG ("del interface %s", if_name.c_str ());
            remove = true;
          }
        break;
      default:
        SRP_LOG_DBG ("unsupported operation %d", op);
        rc = SR_ERR_UNSUPPORTED;
        goto nothing_todo;
      }

    sr_free_val (old_val);
    sr_free_val (new_val);
  }

  if (create)
    {
      // try to split intf_name
      string &intf_name = if_name;
      if (is_af_intf (if_name))
        {
          vec_str splited = split (intf_name, '-');
          if (splited.size () != 2)
            {
              SRP_LOG_ERR ("invalid af packet interface name %s",
                           intf_name.c_str ());
              rc = SR_ERR_INVAL_ARG;
              goto nothing_todo;
            }
          string af_intf_name = splited[1];
          SRP_LOG_DBG ("Creating linux af packet interface %s",
                       af_intf_name.c_str ());
          int rv = create_af_intf (af_intf_name);
          if (rv != 0)
            {
              SRP_LOG_ERR ("cannot create af packet interface %s",
                           intf_name.c_str ());
              rc = SR_ERR_INVAL_ARG;
              goto nothing_todo;
            }
          if (enabled)
            {
              rv = set_intf_status (intf_name, true);
              if (rv != 0)
                {
                  SRP_LOG_ERR ("Cannot enable interface %s",
                               intf_name.c_str ());
                  rc = SR_ERR_INTERNAL;
                  goto nothing_todo;
                }
            }
          sr_free_change_iter (iter);
          return SR_ERR_OK;
        }
    }
  else if (modify)
    {
      SRP_LOG_DBG ("operation is to modify %s set to %s", if_name.c_str (),
                   (enabled ? "enable" : "disable"));

      int rv = set_intf_status (if_name, enabled);
    }
  else if (remove)
    {
      SRP_LOG_INF ("deleting interface '%s'", if_name.c_str ());
      // todo del interface
    }

  sr_free_change_iter (iter);

  return SR_ERR_OK;

nothing_todo:
  sr_free_val (old_val);
  sr_free_val (new_val);
  sr_free_change_iter (iter);
  return rc;
}

static int ipv46_config_add_remove (const string &intf_name,
                                    const string &addr, int prefix_length,
                                    bool add)
{
  rc_t rc = rc_t::OK;
  SRP_LOG_DBG ("in function:%s", __FUNCTION__);

  int rv = -1;
  // todo check if interface exsits
  if (add)
    {
      SRP_LOG_DBG ("add ip %s to intf %s", addr.c_str (), intf_name.c_str ());
      rv = add_intf_ip (intf_name, addr, prefix_length);
      if (rv != 0)
        {
          SRP_LOG_ERR ("add ip %s to intf %s failed", addr.c_str (),
                       intf_name.c_str ());
          return SR_ERR_OPERATION_FAILED;
        }
    }
  else
    {
      SRP_LOG_DBG ("delete ip %s from intf %s", addr.c_str (),
                   intf_name.c_str ());
      rv = del_intf_ip (intf_name, addr, prefix_length);
      if (rv != 0)
        {
          SRP_LOG_ERR ("del ip %s from intf %s failed", addr.c_str (),
                       intf_name.c_str ());
        }
      return SR_ERR_OPERATION_FAILED;
    }

  return SR_ERR_OK;
}

static void parse_interface_ipv46_address (sr_val_t *val, string &addr,
                                           uint8_t &prefix)
{
  if (val == nullptr)
    throw std::runtime_error ("Null pointer");

  if (SR_LIST_T == val->type)
    {
      /* create on list item - reset state vars */
      addr.clear ();
    }
  else
    {
      if (sr_xpath_node_name_eq (val->xpath, "ip"))
        {
          addr = val->data.string_val;
        }
      else if (sr_xpath_node_name_eq (val->xpath, "prefix-length"))
        {
          prefix = val->data.uint8_val;
        }
      else if (sr_xpath_node_name_eq (val->xpath, "netmask"))
        {
          prefix = utils::netmask_to_plen (
              boost::asio::ip::address::from_string (val->data.string_val));
        }
    }
}

/**
 * @brief Callback to be called by any config change in subtrees
 * "/ietf-interfaces:interfaces/interface/ietf-ip:ipv4/address"
 * or "/ietf-interfaces:interfaces/interface/ietf-ip:ipv6/address".
 */
static int ietf_interface_ipv46_address_change_cb (sr_session_ctx_t *session,
                                                   const char *xpath,
                                                   sr_notif_event_t event,
                                                   void *private_ctx)
{
  UNUSED (private_ctx);
  sr_change_iter_t *iter = nullptr;
  sr_change_oper_t op = SR_OP_CREATED;
  sr_val_t *old_val = nullptr;
  sr_val_t *new_val = nullptr;
  sr_xpath_ctx_t xpath_ctx;
  string new_addr, old_addr;
  string if_name;
  uint8_t new_prefix = 0;
  uint8_t old_prefix = 0;
  int rc = SR_ERR_OK, op_rc = SR_ERR_OK;
  bool create = false;
  bool del = false;

  SRP_LOG_INF ("In %s", __FUNCTION__);

  /* no-op for apply, we only care about SR_EV_{ENABLED,VERIFY,ABORT} */
  if (SR_EV_APPLY == event)
    return SR_ERR_OK;

  SRP_LOG_DBG ("'%s' modified, event=%d", xpath, event);

  /* get changes iterator */
  rc = sr_get_changes_iter (session, xpath, &iter);
  if (SR_ERR_OK != rc)
    {
      sr_free_change_iter (iter);
      SRP_LOG_ERR ("Unable to retrieve change iterator: %s", sr_strerror (rc));
      return rc;
    }

  foreach_change (session, iter, op, old_val, new_val)
  {
    if (new_val)
      {
        SRP_LOG_DBG ("new value is not null:%s", new_val->xpath);
      }
    else
      {
        SRP_LOG_DBG ("old value is not null:%s", old_val->xpath);
      }

    SRP_LOG_DBG ("A change detected in '%s', op=%d",
                 new_val ? new_val->xpath : old_val->xpath, op);

    // Looks up the value of the key in a node specified by name.
    if_name = sr_xpath_key_value (new_val ? new_val->xpath : old_val->xpath,
                                  "interface", "name", &xpath_ctx);
    if (if_name.empty ())
      {
        rc = SR_ERR_OPERATION_FAILED;
        goto nothing_todo;
      }
    else
      {
        SRP_LOG_DBG ("Starting to config:%s", if_name.c_str ());
      }
    sr_xpath_recover (&xpath_ctx);

    try
      {
        switch (op)
          {
          case SR_OP_CREATED:
            create = true;
            parse_interface_ipv46_address (new_val, new_addr, new_prefix);
            SRP_LOG_DBG ("add ip %s/%d to intf %s", new_addr.c_str (),
                         new_prefix, if_name.c_str ());
            break;
          case SR_OP_MODIFIED:
            // todo cannot modify ip
            create = true;
            del = true;
            parse_interface_ipv46_address (old_val, old_addr, old_prefix);
            parse_interface_ipv46_address (new_val, new_addr, new_prefix);
            SRP_LOG_DBG ("modify intf %s ip %s/%d to %s/%d", if_name.c_str (),
                         old_addr.c_str (), old_prefix, new_addr.c_str (),
                         new_prefix);
            break;
          case SR_OP_DELETED:
            del = true;
            parse_interface_ipv46_address (old_val, old_addr, old_prefix);
            SRP_LOG_DBG ("from intf %s del ip %s/%d", if_name.c_str (),
                         old_addr.c_str (), old_prefix);
            break;
          default:
            break;
          }
      }
    catch (std::exception &exc)
      {
        SRP_LOG_ERR ("Error: %s", exc.what ());
      }
    sr_free_val (old_val);
    sr_free_val (new_val);

    // if modify,delete and then create
    if (del && !old_addr.empty ())
      {
        SRP_LOG_DBG_MSG ("delete ip address");
        op_rc = ipv46_config_add_remove (if_name, old_addr, old_prefix,
                                         false /* del */);
      }
    if (create && !new_addr.empty ())
      {
        SRP_LOG_DBG_MSG ("add ip address");
        op_rc = ipv46_config_add_remove (if_name, new_addr, new_prefix,
                                         true /* add */);
      }
  }
  sr_free_change_iter (iter);

  return op_rc;

nothing_todo:
  sr_free_val (old_val);
  sr_free_val (new_val);
  sr_free_change_iter (iter);
  return rc;
}

/**
 * @brief Callback to be called by any request for state data under
 * "/ietf-interfaces:interfaces-state/interface" path. Here we reply
 * systematically with all interfaces, it the responsability of sysrepo apply a
 * filter not to answer undesired interfaces.
 */
static int ietf_interface_state_cb (const char *xpath, sr_val_t **values,
                                    size_t *values_cnt, uint64_t request_id,
                                    const char *original_xpath,
                                    void *private_ctx)
{
  UNUSED (request_id);
  UNUSED (original_xpath);
  UNUSED (private_ctx);
  vapi_payload_sw_interface_details interface;
  shared_ptr<interface_dump> dump;
  sr_val_t *val = nullptr;
  int vc = 6;  // number of answer per interfaces
  int cnt = 0; // value counter
  int interface_num = 0;
  int rc = SR_ERR_OK;

  SRP_LOG_INF ("In %s", __FUNCTION__);

  // xpath must end with interface
  if (!sr_xpath_node_name_eq (xpath, "interface"))
    goto nothing_todo; // no interface field specified

  dump = make_shared<interface_dump> ();
  HW::enqueue (dump);
  HW::write ();

  interface_num = std::distance ((*dump).begin (), (*dump).end ());

  /* allocate array of values to be returned */
  SRP_LOG_DBG ("number of interfaces: %d", interface_num + 1);
  rc = sr_new_values ((interface_num + 1) * vc, &val);
  if (0 != rc)
    goto nothing_todo;

  for (auto &it : *dump)
    {
      interface = it.get_payload ();

      SRP_LOG_DBG ("State of interface %s", interface.interface_name);

      /* it needs if-mib YANG feature to work !
       * admin-state: state as required by configuration */
      sr_val_build_xpath (&val[cnt], "%s[name='%s']/admin-status", xpath,
                          interface.interface_name);
      sr_val_set_str_data (&val[cnt], SR_ENUM_T,
                           interface.admin_up_down ? "up" : "down");
      cnt++;

      /* oper-state: effective state. can differ from admin-state */
      sr_val_build_xpath (&val[cnt], "%s[name='%s']/oper-status", xpath,
                          interface.interface_name);
      sr_val_set_str_data (&val[cnt], SR_ENUM_T,
                           interface.link_up_down ? "up" : "down");
      cnt++;

      sr_val_build_xpath (&val[cnt], "%s[name='%s']/phys-address", xpath,
                          interface.interface_name);
      sr_val_build_str_data (&val[cnt], SR_STRING_T,
                             "%02x:%02x:%02x:%02x:%02x:%02x",
                             interface.l2_address[0], interface.l2_address[1],
                             interface.l2_address[2], interface.l2_address[3],
                             interface.l2_address[4], interface.l2_address[5]);
      cnt++;

      sr_val_build_xpath (&val[cnt], "%s[name='%s']/if-index", xpath,
                          interface.interface_name);
      val[cnt].type = SR_INT32_T;
      val[cnt].data.int32_val = interface.sw_if_index;
      cnt++;

      sr_val_build_xpath (&val[cnt], "%s[name='%s']/speed", xpath,
                          interface.interface_name);
      val[cnt].type = SR_UINT64_T;
      val[cnt].data.uint64_val = interface.link_speed;
      cnt++;
    }

  *values = val;
  *values_cnt = cnt;

  return SR_ERR_OK;

nothing_todo:
  *values = nullptr;
  *values_cnt = 0;
  return rc;
}

/**
 * @brief Callback to be called by any request for state data under
 * "/ietf-interfaces:interfaces-state/interface/statistics" path.
 */
static int interface_statistics_cb (const char *xpath, sr_val_t **values,
                                    size_t *values_cnt, uint64_t request_id,
                                    const char *original_xpath,
                                    void *private_ctx)
{
  UNUSED (request_id);
  UNUSED (original_xpath);
  UNUSED (private_ctx);
  shared_ptr<interface> interface;
  interface::stats_t stats;
  string intf_name;
  sr_val_t *val = NULL;
  int vc = 8;
  int cnt = 0; // value counter
  sr_xpath_ctx_t state;
  int rc = SR_ERR_OK;

  SRP_LOG_INF ("In %s", __FUNCTION__);

  /* Retrieve the interface asked */
  intf_name = sr_xpath_key_value ((char *)xpath, "interface", "name", &state);
  if (intf_name.empty ())
    {
      SRP_LOG_ERR_MSG ("XPATH interface name not found");
      return SR_ERR_INVAL_ARG;
    }
  else
    {
      SRP_LOG_DBG ("trying to retrieve interface name %s from xpath",
                   intf_name.c_str ());
    }
  sr_xpath_recover (&state);

  interface = interface::find (intf_name);
  if (interface == nullptr)
    {
      SRP_LOG_WRN ("interface %s not found in VOM", intf_name.c_str ());
      goto nothing_todo;
    }
  else
    {
      SRP_LOG_DBG ("found interface with name %s", intf_name.c_str ());
    }

  /* allocate array of values to be returned */
  rc = sr_new_values (vc, &val);
  if (0 != rc)
    goto nothing_todo;

  stats = interface->get_stats ();

  // if/rx
  sr_val_build_xpath (&val[cnt], "%s/in-octets", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_rx.bytes;
  cnt++;

  // if/rx-unicast
  sr_val_build_xpath (&val[cnt], "%s/in-unicast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_rx_unicast.packets;
  cnt++;

  // if/rx-broadcast
  sr_val_build_xpath (&val[cnt], "%s/in-broadcast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_rx_broadcast.packets;
  cnt++;

  // if/rx-multicast
  sr_val_build_xpath (&val[cnt], "%s/in-multicast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_rx_multicast.packets;
  cnt++;

  // if/tx
  sr_val_build_xpath (&val[cnt], "%s/out-octets", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_tx.bytes;
  cnt++;

  // if/tx-unicast
  sr_val_build_xpath (&val[cnt], "%s/out-unicast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_tx_unicast.packets;
  cnt++;

  // if/tx-broadcast
  sr_val_build_xpath (&val[cnt], "%s/out-broadcast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_tx_broadcast.packets;
  cnt++;

  // if/tx-multicast
  sr_val_build_xpath (&val[cnt], "%s/out-multicast-pkts", xpath, 5);
  val[cnt].type = SR_UINT64_T;
  val[cnt].data.uint64_val = stats.m_tx_multicast.packets;
  cnt++;

  *values = val;
  *values_cnt = cnt;

  return SR_ERR_OK;

nothing_todo:
  *values = NULL;
  *values_cnt = 0;
  return rc;
}

int ietf_interface_init (sc_plugin_main_t *pm)
{
  int rc = SR_ERR_OK;
  SRP_LOG_DBG_MSG ("Initializing ietf-interface plugin.");

  rc = sr_subtree_change_subscribe (pm->session,
                                    "/ietf-interfaces:interfaces/interface",
                                    ietf_interface_create_cb, nullptr, 100,
                                    SR_SUBSCR_CTX_REUSE, &pm->subscription);
  if (SR_ERR_OK != rc)
    {
      goto error;
    }

  rc = sr_subtree_change_subscribe (
      pm->session,
      "/ietf-interfaces:interfaces/interface/ietf-ip:ipv4/address",
      ietf_interface_ipv46_address_change_cb, nullptr, 99, SR_SUBSCR_CTX_REUSE,
      &pm->subscription);
  if (SR_ERR_OK != rc)
    {
      goto error;
    }

  rc = sr_dp_get_items_subscribe (pm->session,
                                  "/ietf-interfaces:interfaces-state",
                                  ietf_interface_state_cb, nullptr,
                                  SR_SUBSCR_CTX_REUSE, &pm->subscription);
  if (SR_ERR_OK != rc)
    {
      goto error;
    }

  //   rc = sr_dp_get_items_subscribe (
  //       pm->session,
  //       "/ietf-interfaces:interfaces-state/interface/statistics",
  //       interface_statistics_cb, NULL, SR_SUBSCR_CTX_REUSE,
  //       &pm->subscription);
  //   if (SR_ERR_OK != rc)
  //     {
  //       goto error;
  //     }

  SRP_LOG_DBG_MSG ("ietf-interface plugin initialized successfully.");
  return SR_ERR_OK;

error:
  SRP_LOG_ERR ("Error by initialization of ietf-interface plugin. Error : %d",
               rc);
  return rc;
}

void ietf_interface_exit (__attribute__ ((unused)) sc_plugin_main_t *pm)
{
}

SC_INIT_FUNCTION (ietf_interface_init);
SC_EXIT_FUNCTION (ietf_interface_exit);
