//
// Created by Administrator on 2024/10/16.
//

#include "wifi_sta.h"
static EventGroupHandle_t wifi_event;

static const char *TAG = "static_ip";

/**
 * @brief   链接显示
 * @param   flag    2->链接，1->链接失败,0->在链接中
 * @retval  无*/
void connect_display(uint8_t flag)
{
    if ( flag == 2 )
    {
        ESP_LOGI(TAG, "链接成功\n");
    }
    else if ( flag == 1 )
    {
        ESP_LOGI(TAG, "链接失败\n");
    }
    else
    {
        ESP_LOGI(TAG, "正在链接中\n");
    }
}

/**
 * @brief   WIFI链接及断开函数
 * @param   event_base  WIFI事件
 * @param   event_id    事件ID
 * @param   event_data  事件数据
 * */
void wifi_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    uint8_t s_retry_num = 0;
    /** 扫描要链接的WIFI事件,当调用WIFI_Start就会触发这个事件 */
    if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START )
    {
        connect_display(0);
        /**可以开始调用wifi连接函数*/
        esp_wifi_connect();
    }
    /**当WIFI连接事件被触发*/
    else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED )
    {
        connect_display(2);
    }
    /**WIFI连接失败事件,在规定的时间内重新启动WIFI传输*/
    else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED )
    {
        /**尝试连接*/
        if ( s_retry_num < 20 )
        {
            s_retry_num++;
            esp_wifi_connect();
            ESP_LOGI(TAG,"正在重新尝试连接WIFI\n");
        }
        else
        {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            connect_display(1);
        }
    }
    /**从连接的IP中获取*/
    else if ( event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP )
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "staic ip" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
    }
}




/**
 * @brief   wifi初始化
 * @param   无
 * @retval  无
 * */
void wifi_sta_init(void)
{
    static esp_netif_t* sta_netif = NULL;
    /**创建一个事件标志组*/
    wifi_event = xEventGroupCreate();
    /**网卡初始化,创建一个LWIP核心任务，并初始化LWIP的相关工作*/
    ESP_ERROR_CHECK(esp_netif_init());
    /**创建一个系统事件任务，并初始化应用程序事件的回调函数*/
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    /**相当于QT的信号与槽*/
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    /**初始化WIFI*/
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = WIFICONFIG();
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    /**等待链接成功后，IP生成*/
    EventBits_t bits = xEventGroupWaitBits(wifi_event, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFAIL, pdFAIL, portMAX_DELAY);
    /**判断对应的连接事件*/
    if ( bits & WIFI_CONNECTED_BIT )
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", DEFAULT_SSID, DEFAULT_PWD);
    }
    else if( bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Filed to connect to SSID:%s, password:%s", DEFAULT_SSID, DEFAULT_PWD);
    }
    vEventGroupDelete(wifi_event);
}
