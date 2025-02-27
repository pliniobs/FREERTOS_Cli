#ifndef CLI_UART_H
#define CLI_UART_H
#include "stm32l4xx_hal.h"

#define CLI_UART_BAUDRATE 115200

extern UART_HandleTypeDef Cli_USART_Handler;

void Cli_Uart_Init(void);

void Cli_Uart_Deinit(void);

#endif  // CLI_UART_H
