//
// Created by Administrator on 2024/11/8.
//

#include "tcp_mclient.h"

/*需要自己设置远程IP地址*/
#define IP_ADDR "192.168.2.6"

#define LWIP_DEMO_RX_BUFSIZE        100         /*最大数据接收长度*/
#define LWIP_DEMO_PORT              8000        /*连接的本地端口号*/
#define LWIP_SEND_THREAD_PRIO       (tskIDLE_PRIORITY + 3)  /*发送数据线程优先级*/

/*接收数据缓冲区*/
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE];

/*发送数据的内容*/
uint8_t g_lwip_demo_sendbuf[] = "shang ni ma de ban!\n";
/*数据发送标志位*/
uint8_t g_lwip_send_flag;
int  g_sock = -1;
int g_lwip_connect_state = 0;
static void lwip_send_thread(void *arg);

/**
 * @brief   发送数据线程
 * @param   无
 * @retval  无
 * */
void lwip_data_send(void )
{
    xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, NULL,LWIP_SEND_THREAD_PRIO, NULL);
}

/**
 * @brief   TCP_Client试验
 * @param   无
 * @param   无
 * */
void tcp_demo(void )
{
    struct sockaddr_in my_client_addr;
    err_t err;
    int recv_data_len;
    char *tbuf;

    lwip_data_send();
    while ( 1 )
    {
    sock_start:
        g_lwip_connect_state = 0;
        my_client_addr.sin_family = AF_INET;                    /*表示IPv4网络协议*/
        my_client_addr.sin_port = htons(LWIP_DEMO_PORT);        /*端口号*/
        my_client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);    /*远程IP地址 */
        g_sock = socket(AF_INET, SOCK_STREAM, 0);   /* 可靠数据流交付服务既是TCP协议 */
        memset(&(my_client_addr.sin_zero), 0, sizeof( my_client_addr.sin_zero ));

        tbuf = malloc(200);                                     /*申请内存*/
        sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);       /*客户端口号*/
        ltdc_show_string(5, 170, 200, 16, 16,tbuf, MAGENTA);

        /*连接远程IP地址*/
        err = connect(g_sock, (struct sockaddr *)&my_client_addr, sizeof( struct sockaddr));

        if ( err == -1 )
        {
            ltdc_show_string(5, 190, 200, 16, 16, "state:Disconnect", MAGENTA);
            g_sock = -1;
            closesocket(g_sock);
            free(tbuf);
            vTaskDelay(10);
            goto  sock_start;
        }

        ltdc_show_string(5, 190, 200, 16, 16, "state:Connection Successful", MAGENTA);
        g_lwip_connect_state = 1;

        while ( 1 )
        {
            recv_data_len = recv(g_sock, g_lwip_demo_recvbuf, LWIP_DEMO_RX_BUFSIZE, 0);
            if ( recv_data_len <= 0 )
            {
                closesocket(g_sock);
                g_sock = -1;
                ltdc_color_fill(5, 190, ltdcdev.width, 320, WHITE);
                ltdc_show_string(5, 190, 200, 16, 16, "State:Disconnect", MAGENTA);
                free(tbuf);
                goto sock_start;
            }
            printf("%s\r\n", g_lwip_demo_recvbuf);
            vTaskDelay(10);
        }
    }
}

/**
 * @brief       发送数据线程函数
 * @param       pvParameters:传入参数
 * @retval      无
 * */
void lwip_send_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    err_t err;
    while ( 1 )
    {
        while(1)
        {
            if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA)&& (g_lwip_connect_state == 1) )    /*有数据要发送*/
            {
                err = write(g_sock, g_lwip_demo_sendbuf, sizeof( g_lwip_demo_sendbuf ));
                if ( err < 0 )
                    break;
                g_lwip_send_flag &=~LWIP_SEND_DATA;
            }
            vTaskDelay(10);
        }
        closesocket(g_sock);
    }
}