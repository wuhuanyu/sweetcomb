#include <string>
#include <memory>

#include "sc_plugins.h"
#include "sys_util.h"
#include "utils/utils.h"
#include "cmds/intfcmd.h"
#include "utils/intfutils.h"

static void dumb_parse_interface_ipv46_address (sr_val_t *val, string &addr,
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

static int dumb_interface_create_cb (sr_session_ctx_t *session,
                                         const char *xpath,
                                         sr_notif_event_t event,
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
            enabled = new_val->data.bool_val;
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
      int rv=0;
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
       
          if (rv != 0)
            {
              SRP_LOG_ERR ("cannot create af packet interface %s",
                           intf_name.c_str ());
              rc = SR_ERR_INVAL_ARG;
              goto nothing_todo;
            }
          if (enabled)
            {
           
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

static int dumb_interface_ipv4_address_change_cb (sr_session_ctx_t *session,
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
            dumb_parse_interface_ipv46_address (new_val, new_addr, new_prefix);
            SRP_LOG_DBG("add intf %s ip %s",if_name.c_str(),new_addr.c_str());
            break;
          case SR_OP_MODIFIED:
            // todo cannot modify ip
            create = true;
            del = true;

            SRP_LOG_DBG ("modify intf %s ip %s/%d to %s/%d", if_name.c_str (),
                         old_addr.c_str (), old_prefix, new_addr.c_str (),
                         new_prefix);
            break;
          case SR_OP_DELETED:
            del = true;
            
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
       
      }
    if (create && !new_addr.empty ())
      {
        SRP_LOG_DBG_MSG ("add ip address");
      
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

int dumb_interface_init (sc_plugin_main_t *pm)
{
  int rc = SR_ERR_OK;
  SRP_LOG_DBG_MSG ("Initializing dumb ietf-interface plugin.");

  //订阅的优先级问题file:///private/tmp/html/subscribtions.html
  //优先级高的先订阅
  rc = sr_subtree_change_subscribe (pm->session,
                                    "/ietf-interfaces:interfaces/interface",
                                    dumb_interface_create_cb, nullptr, 100,
                                    SR_SUBSCR_CTX_REUSE, &pm->subscription);
  if (SR_ERR_OK != rc)
    {
      goto error;
    }

  rc = sr_subtree_change_subscribe (
      pm->session,
      "/ietf-interfaces:interfaces/interface/ietf-ip:ipv4/address",
      dumb_interface_ipv4_address_change_cb, nullptr, 99, SR_SUBSCR_CTX_REUSE,
      &pm->subscription);
  if (SR_ERR_OK != rc)
    {
      goto error;
    }

  SRP_LOG_DBG_MSG ("Dumb ietf-interface plugin initialized successfully.");
  return SR_ERR_OK;

error:
  SRP_LOG_ERR ("Error by initialization of ietf-interface plugin. Error : %d",
               rc);
  return rc;
}

void dumb_interface_exit (__attribute__ ((unused)) sc_plugin_main_t *pm)
{
}

// SC_INIT_FUNCTION (dumb_interface_init);
// SC_EXIT_FUNCTION (dumb_interface_exit);
