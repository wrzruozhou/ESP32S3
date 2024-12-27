//
// Created by Administrator on 2024/11/11.
//

#include "mqtt_test.h"


int g_publish_flag = 0;         /*发布成功标志位*/
static const char *TAG = "MQTT_EXAMPLE";
char g_lcd_mqtt_buf[100] = {0};

/**
 * @brief   错误日记
 * @param   message :错误消息
 * @param   error_code: 错误码
 * @retval  无
 * */
static void log_error_if_nonzero(const char *message, int error_code)
{
    if ( error_code != 0 )
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/**
 * @brief   注册接收MQTT事件的事件处理程序
 * @param   handler_args:注册到事件的用户数据
 * @param   base    :处理程序的事件库
 * @param   event_id:接收到的事件id
 * @param   event_data 事件的数据
 * @retval  无
 * */
void mqtt_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) /**< function called when an event is posted to the queue */
{
    ESP_LOGD(TAG,"Event dispatched from event from vent loop base=%s, event_id=%" PRIi32 "",event_base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ( (esp_mqtt_event_id_t)event_id )
    {
        case MQTT_EVENT_CONNECTED:  /*连接事件*/
            ESP_LOGI(TAG,"MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(TAG,"sent publish successful, msg_id=%d",msg_id);

            msg_id = esp_mqtt_client_subscribe(client,"/topic/qos1",0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            g_publish_flag = 1;
            /**订阅主题*/
            msg_id = esp_mqtt_client_subscribe(client, DEVICE_SUBSCRIBE, 0);
            ESP_LOGI(TAG, "sent subscrive successful, msg_id=%d", msg_id);
        case MQTT_EVENT_ANY:
            break;
        case MQTT_EVENT_ERROR:              /*错误事件*/
            if ( event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT )
            {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        case MQTT_EVENT_DISCONNECTED:       /*断开连接事件*/
            break;
        case MQTT_EVENT_SUBSCRIBED:         /*订阅事件*/
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:       /*取消订阅事件*/
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:          /*发布事件*/
            ESP_LOGI(TAG,"MQTT_EVENT_PUBLISHED, msg_id=%d",event->msg_id);
            break;
        case MQTT_EVENT_DATA:               /*接收数据事件*/
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            break;
        case MQTT_EVENT_DELETED:
            break;
        case MQTT_USER_EVENT:
            break;
    }
}


/**
 * @brief   进程
 * @param   无
 * @param   无
 * */
void mqtt_demo(void )
{
    char mqtt_publish_data[] = "烧烤唐老鸭";
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = HOST_NAME,                               /*MQTT地址*/
        .broker.address.port = HOST_PORT,                                   /*MQTT端口号*/
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,                /*TCP模式*/
        .credentials.client_id = CLIENT_ID,                                 /*设备名称*/
        .credentials.username = (char *)USER_NAME,                          /*产品ID*/
        .credentials.authentication.password = PASSWORD,                    /*计算出来的密码*/
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    while ( 1 )
    {
        if ( g_publish_flag == 1 )
        {
            g_publish_flag = 0;
            esp_mqtt_client_publish(client, DEVICE_PUBLISH, (char *)mqtt_publish_data, strlen(mqtt_publish_data),1,0);
        }
        vTaskDelay(100);
    }
}