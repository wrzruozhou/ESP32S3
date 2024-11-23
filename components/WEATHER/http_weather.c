//
// Created by Administrator on 2024/11/23.
//

#include "http_weather.h"
static const char *TAG = "HTTP_CLIENT";

/**
 * @brief   通过http调用api获取天气的任务
 * @param   pvParameters:任务数据句柄
 * @retval  无
 * */
void http_test_task(void *pvParameters)
{
    /*定义所需要的变量*/
    char output_buffer[MAX_HTTP_OUTPUT_BUFF] = {0};     /*用于接收通过http协议返回的数据*/
    int64_t content_length = 0;                             /*http协议头的长度*/

    /*配置http结构体*/
    /*定义http配置结构体,并且进行清零*/
    esp_http_client_config_t config = {0};
    memset(&config, 0, sizeof(config));

    /*向配置结构体写入URL*/
    /*api.seniverse.com/v3/weather/now.json?key=SVpB6e1oXZG2IstmE&location=beijing&language=zh-Hans&unit=c*/
    static const char *URL = "https://api.seniverse.com/v3/weather/now.json?key=SVpB6e1oXZG2IstmE&location=shenzhen&language=zh-Hans&unit=c";
    config.url = URL;
    config.auth_type = HTTP_AUTH_TYPE_NONE;
//    config.url = "https://api.seniverse.com/v3/weather/now.json?key=SVpB6e1oXZG2IstmE&location=beijing&language=zh-Hans&unit=c";

    /*初始化http*/
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if ( client == NULL )
    {
        ESP_LOGE(TAG,"client_init error");
    }

    /*配置发送请求*/
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    /*循环通讯*/
    while ( 1 )
    {
        /*与目标主机创建连接，并声明写入内容长度为0*/
        esp_err_t err = esp_http_client_open(client, 0);

        /*如果连接失败*/
        if ( err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to open http connect!: %s", esp_err_to_name(err));
        }
        else    /*如果连接成功*/
        {
            /*读取目标主机返回的协议头*/
            content_length = esp_http_client_fetch_headers(client);
            /*如果小于0说明没有成功读取到*/
            if ( content_length < 0 )
            {
                ESP_LOGE(TAG, "Failed to fetch headers");
            }
            else
            {
                /*读取目标主机通过http的响应内容*/
                int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFF);
                if ( data_read >= 0 )
                {
                    /*打印响应的内容*/
                    ESP_LOGI(TAG, "http GET STATUS = %d, content_length = %lld",
                              esp_http_client_get_status_code(client),         /*获取响应状态信息*/
                              esp_http_client_get_content_length(client));      /*获取响应信息长度*/
                    /*这个代码测试用*/
//                    printf("data:%s\n", output_buffer);

                    /*下面进行数据处理*/
//                    cJSON *root = NULL;
                    cJSON *root = cJSON_Parse(output_buffer);
                    if ( root == NULL )
                    {
                        ESP_LOGE(TAG, "Error parsing JSON data!\n");
                    }
                    /*获取results标题*/
                    cJSON *results = cJSON_GetObjectItem(root, "results");
                    if ( results == NULL )
                    {
                        ESP_LOGE(TAG, "failed to get 'results' object!\n");
                        cJSON_Delete(results);
                    }

                    cJSON *location = cJSON_GetObjectItem(results->child, "location");
                    if ( location == NULL )
                    {
                        ESP_LOGE(TAG, "failed to get 'location' object!\n");
                        cJSON_Delete(location);
                    }

                    cJSON *now = cJSON_GetObjectItem(results->child, "now");
                    if( now == NULL )
                    {
                        ESP_LOGE(TAG, "failed to get 'now' object!\n");
                        cJSON_Delete(now);
                    }

                    const char *name = cJSON_GetObjectItem(location, "name")->valuestring;
                    const char *text = cJSON_GetObjectItem(now, "text")->valuestring;
                    const char *temperature = cJSON_GetObjectItem(now, "temperature")->valuestring;

                    ESP_LOGI(TAG,"location: %s",name);
                    ESP_LOGI(TAG,"Weather: %s", text);
                    ESP_LOGI(TAG,"temperature: %s",temperature);
                }
                else
                {
                    ESP_LOGE(TAG,"Can't Get Respone, please retry again!\n");
                }
            }
        }
        esp_http_client_close(client);
        vTaskDelay(3000);
    }
}