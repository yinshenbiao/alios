/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "aos/yloop.h"
#include "uagent/uagent.h"
#include "ulog/ulog.h"

#include "netmgr.h"

#include "linkkit/dev_model_api.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/infra/infra_config.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_types.h"
#include "linkkit/wrappers/wrappers.h"

#include "linkkit_client.h"

#define MQTT_MSGLEN (1024)

#define EXAMPLE_TRACE(fmt, ...)                        \
    do                                                 \
    {                                                  \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)

extern void app_start(void);
extern void app_init(void);

static char app_started = 0;
static iotx_linkkit_dev_meta_info_t master_meta_info;
static int master_devid = 0;

void *pclient = NULL;
typedef void (*task_fun)(void *);

/* wifi event */
static void wifi_service_event(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI)
    {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP)
    {
        return;
    }

    netmgr_ap_config_t config;
    memset(&config, 0, sizeof(netmgr_ap_config_t));
    netmgr_get_ap_config(&config);
    LOG("wifi_service_event config.ssid %s", config.ssid);
    if (strcmp(config.ssid, "adha") == 0 || strcmp(config.ssid, "aha") == 0)
    {
        // clear_wifi_ssid();
        return;
    }

    if (!app_started)
    {
        aos_task_new("iotx_example", (task_fun)app_start, NULL, 1024 * 6);
        app_started = 1;
    }
}

/* start connecting to wifi */
int wifi_start(const char *ssid, const char *passwd)
{
    netmgr_start(false);
    netmgr_ap_config_t apconfig;

    netmgr_init();
    aos_msleep(100);
    // be_debug(MOD, "%s:ssid=%s\n\r", __FUNCTION__, config->ssid);
    memset(&apconfig, 0, sizeof(apconfig));
    strcpy(apconfig.ssid, ssid);
    strcpy(apconfig.pwd, passwd);
    netmgr_set_ap_config(&apconfig);
    netmgr_start(false);
}

/* start connecting to AliYun IoT Platform */
void linkkit_connect(const char *pk, const char *dn, const char *ds,
                     const char *ps,
                     int (*callback)(const int, const char *, const int), int (*callback2)(void))
{
    int res = 0;

    /* set device certification information */
    HAL_SetProductKey((char *)pk);
    HAL_SetProductSecret((char *)ps);
    HAL_SetDeviceName((char *)dn);
    HAL_SetDeviceSecret((char *)ds);

    memset(&master_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));
    memcpy(master_meta_info.product_key, pk, strlen(pk));
    memcpy(master_meta_info.product_secret, ps, strlen(ps));
    memcpy(master_meta_info.device_name, dn, strlen(dn));
    memcpy(master_meta_info.device_secret, ds, strlen(ds));

    /* set property set event callback function */
    if(callback){
        IOT_RegisterCallback(ITE_PROPERTY_SET, callback);
    }

    /* set connect IoT platform sunccess event callback function */
    if(callback2){
        IOT_RegisterCallback(ITE_CONNECT_SUCC, callback2);
    }

    /* create Master Device Resources */
    master_devid =
        IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &master_meta_info);
    if (master_devid < 0)
    {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return;
    }

    /* start connect Aliyun Server */
    res = IOT_Linkkit_Connect(master_devid);
    if (res < 0)
    {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return;
    }
}

/* setting device properties */
int linkkit_set_props(unsigned char *payload, int payload_len)
{
    return IOT_Linkkit_Report(master_devid, ITM_MSG_POST_PROPERTY, payload,
                              payload_len);//设备ID，设备属性数据上报，消息Payload，消息Payload的长度
    //向云端发送消息, 包括属性上报/设备标签信息更新上报/设备标签信息删除上报/透传数据上报/子设备登录/子设备登出
}

void linkkit_start(void)
{
    while (1)
    {
        IOT_Linkkit_Yield(200);
    }
}

int application_start(int argc, char **argv)
{
#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    aos_set_log_level(AOS_LL_DEBUG);

    aos_register_event_filter(EV_WIFI, wifi_service_event, NULL);

    app_init();

    aos_loop_run();

    return 0;
}