//
// Created by Administrator on 2024/6/20.
//

#ifndef __UART_H
#define __UART_H

#include "driver/gpio.h"
#include "driver/uart.h"

#define USART_UX UART_NUM_0
#define USART_TX_GPIO_PIN GPIO_NUM_43
#define USART_RX_GPIO_PIN GPIO_NUM_44

#define RX_BUF_SIZE 1024

void uart_init(uint32_t baud);

#endif//WRZ_UART_H
