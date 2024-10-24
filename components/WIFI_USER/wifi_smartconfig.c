//
// Created by Administrator on 2024/10/24.
//

#include "wifi_smartconfig.h"



/**定义事件*/
static EventGroupHandle_t s_wifi_event_group;
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "smartconfig_example";
static void smartconfig_task(void *parm);
static char lcd_buff[100] = {0};

/**< function called when an event is posted to the queue */
/**
 * @brief   WIFI事件处理函数
 * @param   arg:            传入网卡的控制块
 * @param   event_base:     WIFI事件
 * @param   event_id:       事件ID
 * @param   event_data      事件数据
 * @retval  无*/
void event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START )
    {
        xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
    }else if ( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED ){
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    }else if ( event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP ){
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }else if ( event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE){
        ESP_LOGI(TAG, "SCAN Done");
        ltdc_show_string(0, 90, 320, 16, 16, "In the distribution network......", BLUE);
    }else if ( event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL ){
        ESP_LOGI(TAG, "Found channel");
    }else if( event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD){/**获取SSID和密码,表示配网成功*/
        ESP_LOGI(TAG, "Got SSID and passwdord");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof( wifi_config_t ));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));

#ifdef CONFIG_SET_MAC_ADDRESS_OF_TARGET_AP
        wifi_config.sta.bssid_set = evt->bssid_set;
        if ( wifi_config.sta.bssid_set == true )
        {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }
#endif
        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "SSID:%s", password);

        ltdc_color_fill(0,90,320,240,WHITE);
        sprintf(lcd_buff, "%s",ssid);
        ltdc_show_string(0, 90, 320, 16, 16, lcd_buff, BLUE);
        sprintf(lcd_buff, "%s",password);
        ltdc_show_string(0, 110, 320, 16, 16, lcd_buff, BLUE);
        ltdc_show_string(0, 130, 320, 16, 16, "Successful distribution network", BLUE);

        /** 手机APP ESPTouch使用ESPTOUCH V2模式,会执行以下代码 */
        if ( evt->type == SC_TYPE_ESPTOUCH_V2 ){
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for ( int i = 0; i < 33; ++i )
            {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    }else if(event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE){
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

/**
 * @brief   WIFI一键配网
 * @param   无
 * @retval  无
 * */
void wifi_smartconfig_sta(void )
{
    /**初始化网卡*/
    ESP_ERROR_CHECK(esp_netif_init());
    /**创建事件组*/
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /**把WIFI网卡设置为STA模式*/
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    /**创建WIFI的默认配置*/
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));

    /**仿照QT,创建信号和槽,注册WIFI事件*/
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_start());
}

static void smartconfig_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start( &cfg));
    while ( 1 )
    {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT|ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if ( uxBits & CONNECTED_BIT )
        {
            ESP_LOGI(TAG, "WIFI Connetcted to ap");
        }
        if ( uxBits & ESPTOUCH_DONE_BIT )
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}