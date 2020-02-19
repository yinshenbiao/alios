/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <fcntl.h>

#include <aos/kernel.h>
#include "aos/cli.h"
#include <k_api.h>
#include "sensor/sensor.h"

#include "linkkit/dev_model_api.h"
#include "linkkit/infra/infra_compat.h"
#include "linkkit/infra/infra_config.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_types.h"
#include "linkkit/wrappers/wrappers.h"

#include "cJSON.h"
#include "linkkit_client.h"

#define PRODUCT_KEY "a1jvfVfmGu5"
#define PRODUCT_SECRET "f1V3Oj6W7dFaBU1c"
#define DEVICE_NAME "developerkit_01"
#define DEVICE_SECRET "QvAXNx8ES1G3tLU5HjrvY2tQSgRVkfoD"

/* modify to your wifi information */
#define WIFI_SSID "TP-LINK_0F08"
#define WIFI_PASSWD "qwer123456789"

/* sensor fd */
static int acc_fd = -1;
static aos_timer_t post_timer;
static char post_payload[256] = {0};

static void acc_sensor_post_task(void *arg)
{
    ssize_t size = 0;
    accel_data_t data = {0};
    float acc_x = 0;
    float acc_y = 0;
    float acc_z = 0;

    while(1){
        /* get acc sensor data */
        size = aos_read(acc_fd, &data, sizeof(data));
        if (size != sizeof(data))
        {
            return -1;
        }

        acc_x = ((float)data.data[0] / 1000.0f) * 9.8f;
        acc_y = ((float)data.data[1] / 1000.0f) * 9.8f;
        acc_z = ((float)data.data[2] / 1000.0f) * 9.8f;

        /* post data to IoT platform */
        memset(post_payload, 0, 256);//将post_payload中当前位置后面的256个字节用0替换并返回post_payload 
        sprintf(post_payload, "{\"acc_x\":%.1f,\"acc_y\":%.1f,\"acc_z\":%.1f}", acc_x, acc_y, acc_z);
        //把格式化的数据写入某个字符串中，在完成其他数据类型转换成字符串类型的操作中应用广泛
        printf("[acc_sensor_post_task]:%s\r\n", post_payload);
        linkkit_set_props(post_payload, strlen(post_payload));

        /* delay for 500ms */
        aos_msleep(500);
    }
}

/* sensor init */
static void acc_sensor_init(void)
{
    printf("acc_sensor_init...\r\n");
    /* open acc sensor */
    acc_fd = aos_open(DEV_ACC_PATH(0), O_RDWR);//是对外的接口，外部函数可以直接使用该接口实现对于文件的打开操作，而不用去关心底层文件系统的实现细节。

    if (acc_fd < 0)
    {
        printf("acc sensor open failed !\n");
    }
}

/* IoT platform connected callback 物联网联网平台 */
int linkkit_connect_ok(void)
{
    /* create a task to post acc sensor data to IoT platform 创建一个任务，将acc传感器数据发布到物联网平台*/
    aos_task_new("sensor_post", acc_sensor_post_task, NULL, 4096);//创建一个任务“sensor_post”，配网模式函数
    return 0;
}

/* application init, including sensor, wifi. 应用init，包括传感器，wifi。*/
void app_init(void)
{
    acc_sensor_init();
    wifi_start(WIFI_SSID, WIFI_PASSWD);
}

/* start connecting to IoT platform */
void app_start(void)
{
    linkkit_connect(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, PRODUCT_SECRET, NULL, linkkit_connect_ok);

    linkkit_start();
}