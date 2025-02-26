#ifndef CLI_UART_H
#define CLI_UART_H
#include "stm32l4xx_hal.h"

#define RX_TX_BUFFER_SIZE 256

#define CLI_UART_BAUDRATE 115200

typedef struct {
  uint8_t Input_char;
  uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
  uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
  uint16_t rx_counter;
  uint16_t tx_counter;
} Cli_USART_Data_t;

extern UART_HandleTypeDef Cli_USART_Handler;
extern Cli_USART_Data_t Cli_Usart_Data;
void Cli_Uart_Init(void);

void Cli_Uart_Deinit(void);

#endif  // CLI_UART_H
