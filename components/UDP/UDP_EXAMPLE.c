//
// Created by Administrator on 2024/10/30.
//

#include "UDP_EXAMPLE.h"

/**需要自己远程设置IP地址*/
#define IP_ADDR "192.168.2.6"

#define LWIP_DEMO_RX_BUFSIZE    200    /**最大接收数据长度*/
#define LWIP_DEMO_PORT          8080    /**连接的本地端口号*/
#define LWIP_SEND_THREAD_PRIO   (tskIDLE_PRIORITY + 3)

/* 接收数据缓冲区 */
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE];
/* 发送数据内容 */
char g_lwip_demo_sendbuf[] = "fuck you BLG!\r\n";
/**发送数据标志位*/
//uint8_t g_lwip_send_flag;
static struct sockaddr_in dest_addr;                    /* 远端地址 */
struct sockaddr_in g_local_info;
socklen_t g_sock_fd;                            /* 定义一个Socket接口 */
static void lwip_send_thread(void *pvParameters);
extern QueueHandle_t g_display_queue;           /* 显示消息队列句柄 */


/**
 * @brief   发送数据
 * @param   无
 * @param   无
 * */
void lwip_data_send(void)
{
    xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, NULL, LWIP_SEND_THREAD_PRIO, NULL);
}

/**
 * @brief   lwip_demo试验入口
 * @param   无
 * @param   无
 * */
void lwip_demo(void )
{
    char *tbuf;
    lwip_data_send();
    /**远端参数设置*/
    dest_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(LWIP_DEMO_PORT);

    g_local_info.sin_family = AF_INET;                          /* IPv4地址 */
    g_local_info.sin_port = htons(LWIP_DEMO_PORT);              /* 设置端口号 */
    g_local_info.sin_addr.s_addr = htons(INADDR_ANY);           /* 设置本地IP地址 */

    g_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);                 /* 建立一个新的socket连接 */

    tbuf = malloc(200);                                         /* 申请内存 */
    sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);           /* 客户端端口号 */
    ltdc_show_string(0, 170, 200, 16, 16, tbuf, MAGENTA);
    /* 建立绑定 */
    bind(g_sock_fd, (struct sockaddr *)&g_local_info, sizeof(g_local_info));
    while (1)
    {
        memset(g_lwip_demo_recvbuf, 0, sizeof(g_lwip_demo_recvbuf));
        recv(g_sock_fd, (void *)g_lwip_demo_recvbuf, sizeof(g_lwip_demo_recvbuf), 0);
        printf("%s\r\n",g_lwip_demo_recvbuf);
    }
}

/**
 * @brief       发送数据线程函数
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lwip_send_thread(void *pvParameters)
{
//    pvParameters = pvParameters;

    while (1)
    {
        if ((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA)      /* 有数据要发送 */
        {
            printf("send\r\n");
            sendto(g_sock_fd,                                           /* scoket */
                    (char *)g_lwip_demo_sendbuf,                          /* 发送的数据 */
                    sizeof(g_lwip_demo_sendbuf), 0,                       /* 发送的数据大小 */
                    (struct sockaddr *)&dest_addr,                        /* 接收端地址信息 */
                    sizeof(dest_addr));                                   /* 接收端地址信息大小 */

            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }

        vTaskDelay(100);
    }
}
