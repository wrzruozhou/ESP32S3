//
// Created by Administrator on 2024/11/9.
//

#include "tcp_mserver.h"

#define LWIP_DEMO_RX_BUFSIZE    200         /*最大接收数据长度*/
#define LWIP_DEMO_PORT          8080        /*连接的本地端口号*/
#define LWIP_SEND_THREAD_PRIO   (tskIDLE_PRIORITY + 3)  /*发送数据线程优先级*/

/*接收数据缓冲区*/
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE];
/*发送数据内容*/
uint8_t g_lwip_demo_sendbuf_server[] = "fuck you mother!\r\n";

/*发送数据标志位*/
uint8_t g_lwip_send_flag_server;
int g_sock_conn;                        /*请求的socked*/
int g_lwip_connect_state_server = 0;
static void lwip_send_thread(void *arg);

/**
 * @brief   发送数据线程
 * @param   无
 * @retval  无
 * */
void lwip_data_send(void)
{
    xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, NULL, LWIP_SEND_THREAD_PRIO, NULL);
}

/**
 * @brief   lwip_demo试验入口
 * @param   无
 * @retval  无
 * */
void server_demo(void )
{
    struct sockaddr_in server_addr;     /*服务器地址*/
    struct sockaddr_in conn_addr;       /*连接地址*/
    socklen_t addr_len;                 /*地址长度*/
    int err;
    int length;
    int sock_fd;
    char *tbuf;
    lwip_data_send();                               /*创建一个发送线程*/

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    /* 建立一个新的socket连接 */
    memset(&server_addr, 0, sizeof(server_addr));                                   /*将服务器地址清空*/
    server_addr.sin_family = AF_INET;                                               /*地址家族*/
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);                                /*注意转化为网络字节序*/
    server_addr.sin_port = htons(LWIP_DEMO_PORT);                                   /*使用SERVER_PORT指定为程序头设定的端口号*/

    tbuf = malloc(200);                                                             /*申请内存*/
    sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);       /* 客户端端口号 */
    ltdc_show_string(5, 170, 200, 16, 16, tbuf, BLUE);

    err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof( server_addr ));
    if (err < 0)                /* 如果绑定失败则关闭套接字 */
    {
        closesocket(sock_fd);   /* 关闭套接字 */
        free(tbuf);
    }

    err = listen(sock_fd, 4);
    if (err < 0)                /* 如果监听失败则关闭套接字 */
    {
        closesocket(sock_fd);   /* 关闭套接字 */
    }

    while ( 1 )
    {
        g_lwip_connect_state_server = 0;
        addr_len = sizeof(struct sockaddr_in );     /*将连接第一个地址复制给addr_led*/
        g_sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);/* 对监听到的请求进行连接，状态赋值给sock_conn */

        if ( g_sock_conn < 0 )  /* 状态小于0代表连接故障，此时关闭套接字 */
        {
            closesocket(sock_fd);
        }
        else
        {
            ltdc_show_string(5, 190, 200, 16, 16, "State:Connection...", BLUE);
            g_lwip_connect_state_server = 1;
        }

        while ( 1 )
        {
            memset(g_lwip_demo_recvbuf, 0, LWIP_DEMO_RX_BUFSIZE);
            length = recv(g_sock_conn, (unsigned int *)g_lwip_demo_recvbuf, sizeof( g_lwip_demo_recvbuf ), 0);

            if ( length <= 0 )
            {
                ltdc_show_string(5, 190, 200, 16, 16, "State:no Connection", BLUE);
                break;
            }
            printf("%s\r\n",g_lwip_demo_recvbuf);
        }

        if ( g_sock_conn >= 0 )
        {
            closesocket(g_sock_conn);
        }
        g_sock_conn = -1;
    }
}

/**
 * @brief   发送数据线程函数
 * @param   pvParameters:传入参数(未用到)
 * @retval  无
 * */
void lwip_send_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    while ( 1 )
    {
        if ( ((g_lwip_send_flag_server & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state_server == 1) )    /*有数据要发送*/
        {
            send(g_sock_conn, g_lwip_demo_sendbuf_server, sizeof(g_lwip_demo_sendbuf_server),0);  /*发送数据*/
            g_lwip_send_flag_server &=~ LWIP_SEND_DATA;
        }
        vTaskDelay(1);
    }
}