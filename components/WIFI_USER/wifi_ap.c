//
// Created by Administrator on 2024/10/18.
//

#include "wifi_ap.h"



static const char *TAG_AP = "WiFi SoftAP";

/**
 * @brief   WIFI的连接与断开事件处理函数
 * @param   event_handler_arg:传入的网卡控制块
 * @param   event_base:WIFI事件
 * @param   event_id:事件ID
 * @param   event_data:事件数据
 * @retval  无
 * */
/**< function called when an event is posted to the queue */
void wifi_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED )                       /**有设备接入*/
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_AP, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if ( event_base == WIFI_EVENT && event_id ==  WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_AP, "Station " MACSTR " exit, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/**
 * @brief   WIFI初始化
 * @param   无
 * @param   无
 * */
void wifi_init_softap(void )
{
    /**初始化网卡*/
    ESP_ERROR_CHECK(esp_netif_init());
    /**创建新的事件回环*/
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /**创建一个默认的AP接入点*/
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));
    /**注册一个WIFI事件*/
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler,NULL));

    /**配置WIFI*/
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASSWD,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .max_connection = EXMAPLE_ESP_MAXIMUM_RETRY,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    /**如果WIFI没有密码*/
    if ( strlen( EXAMPLE_ESP_WIFI_PASSWD ) == 0 )
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info( esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI(TAG_AP, "Set up softAP with IP: %s",ip_addr);
    ESP_LOGI(TAG_AP,"wifi_init_softap finished... SSID:'%s' passwd:'%s'", EXAMPLE_ESP_WIFI_SSID,EXAMPLE_ESP_WIFI_PASSWD);

}