//
// Created by Administrator on 2024/10/14.
//

#include "wifi_scan.h"
#include <esp_log.h>
static const char *TAG = "scan";

/**
 * @brief       身份认证模式
 * @param       authmode :身份验证模式
 * @retval      无
 */
static void print_auth_mode(int authmode)
{
    switch (authmode)
    {
        case WIFI_AUTH_OPEN:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
            break;
        case WIFI_AUTH_OWE:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
            break;
        case WIFI_AUTH_WEP:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
            break;
        case WIFI_AUTH_WPA_PSK:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
            break;
        case WIFI_AUTH_WPA2_PSK:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
            break;
        case WIFI_AUTH_WPA3_PSK:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
            break;
        default:
            ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
            break;
    }
}

/**
 * @brief       打印WIFI密码类型
 * @param       pairwise_cipher :密码类型
 * @param       group_cipher    :群密码类型
 * @retval      无
 */
static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher)
    {
        case WIFI_CIPHER_TYPE_NONE:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
            break;
        case WIFI_CIPHER_TYPE_WEP40:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
            break;
        case WIFI_CIPHER_TYPE_WEP104:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
            break;
        case WIFI_CIPHER_TYPE_TKIP:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
            break;
        case WIFI_CIPHER_TYPE_CCMP:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
            break;
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
            break;
        default:
            ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
            break;
    }

    switch (group_cipher)
    {
        case WIFI_CIPHER_TYPE_NONE:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
            break;
        case WIFI_CIPHER_TYPE_WEP40:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
            break;
        case WIFI_CIPHER_TYPE_WEP104:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
            break;
        case WIFI_CIPHER_TYPE_TKIP:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
            break;
        case WIFI_CIPHER_TYPE_CCMP:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
            break;
        case WIFI_CIPHER_TYPE_TKIP_CCMP:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
            break;
        default:
            ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
            break;
    }
}

/**
 * @brief       将Wi-Fi初始化为sta并设置扫描方法
 * @param       无
 * @retval      无
 */
void wifi_scan(void)
{
    char lcd_buff[100] = {0};
    /* 网卡初始化 */
    ESP_ERROR_CHECK(esp_netif_init());
    /* 创建新的事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /* 用户初始化STA模式 */
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    /* wifi配置初始化 */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));
    /* 设置WIFI为STA模式 */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    /* 启动WIFI */
    ESP_ERROR_CHECK(esp_wifi_start());
    /* 开始扫描附件的WIFI */
    esp_wifi_scan_start(NULL, true);
    /* 获取上次扫描中找到的AP列表 */
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    /* 获取上次扫描中找到的AP数量 */
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
    /* 下面是打印附件的WIFI信息 */
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
    {
        sprintf(lcd_buff, "%s",ap_info[i].ssid);
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        print_auth_mode(ap_info[i].authmode);

        if (ap_info[i].authmode != WIFI_AUTH_WEP)
        {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }

        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
    }
}