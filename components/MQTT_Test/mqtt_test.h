//
// Created by Administrator on 2024/11/11.
//

#ifndef WRZ_MQTT_TEST_H
#define WRZ_MQTT_TEST_H

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sys/socket.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "mqtt_client.h"
#include "esp_netif.h"

extern int g_publish_flag;         /*发布成功标志位*/

#define PRODUCT_KEY     "k1z5ffxtFcl"                                                   /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME     "esp32s3_test"                                                  /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET   "7926f41f44191adf2bb008acb2f77c1f"                              /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */

/* MQTT地址与端口 */
//#define HOST_NAME           PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_NAME           "iot-06z00ezkgcdlu9i.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           "12345|securemode=2,signmethod=hmacsha1,timestamp=1731467609168|"  /* 客户端ID */
#define USER_NAME           "esp32s3_test&k1z5ffxtFcl"                                                              /* 客户端用户名 */
#define PASSWORD            "D30B7CC1708F56D0E593FBA6DF594A1909245ABF"                      /* 由MQTT_Password工具计算得出的连接密码 */

/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/gengxing"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/tempture"                             /* 订阅 */
//#define DEVICE_SUBSCRIBE    "风神"                             /* 订阅 */

void mqtt_demo(void );


#endif   // WRZ_MQTT_TEST_H
