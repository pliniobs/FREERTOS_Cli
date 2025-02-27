#include "Cli_Task.h"

#include <stdio.h>

#include "Cli_Uart.h"
#include "FreeRTOS.h"
#include "main.h"
#include "shell.h"
#include "stm32l4xx_hal.h"
#include "task.h"

// Task Handle
TaskHandle_t xTask_Cli_Handler;

// A simple Hello World command which prints "Hello World!"
int cli_cmd_hello(int argc, char *argv[]) {
  shell_put_line("Hello World!");
  return 0;
}

static const sShellCommand s_shell_commands[] = {
    {"hello", cli_cmd_hello, "Say hello"},
    {"help", shell_help_handler, "Lists all commands"},
};
const sShellCommand *const g_shell_commands = s_shell_commands;
const size_t g_num_shell_commands = ARRAY_SIZE(s_shell_commands);

/* Function Prototype */
void VTask_Cli_Manager(void *pvParameters);
void VTask_Cli_Init_Usart(void);
int consolePutchar(char c);

void Init_Cli_Task(void) {
  BaseType_t xReturned;

  Cli_Uart_Init();

  xReturned = xTaskCreate(VTask_Cli_Manager, "CLI Task", 4096, NULL, 1, &xTask_Cli_Handler);
  if (xReturned != pdPASS)
    while (1)
      ;
}

void VTask_Cli_Manager(void *pvParameters) {
  uint32_t Notification_Value;

  // Configure shell
  sShellImpl shell_impl = {
      .send_char = consolePutchar,
  };

  shell_boot(&shell_impl);

  while (1) {
    if (xTaskNotifyWait(0, 0, &Notification_Value, 1000) == pdPASS) {
      shell_receive_char((char)(Notification_Value & 0xFF));
      //      Cli_Usart_Data.tx_counter = snprintf((char *)Cli_Usart_Data.tx_buffer, RX_TX_BUFFER_SIZE, "Received %d: %s\r\n", Cli_Usart_Data.rx_counter, Cli_Usart_Data.rx_buffer);
      //      HAL_UART_Transmit(&Cli_USART_Handler, Cli_Usart_Data.tx_buffer, Cli_Usart_Data.tx_counter, 1000);
      //
      //      /*
      //       * Prepare the usart to receive a new command
      //       * */
      //      Cli_Usart_Data.rx_counter = 0;
      //      HAL_UART_Receive_IT(&Cli_USART_Handler, &Cli_Usart_Data.Input_char, 1);  // Prepare the UART to receive the next character
    } else {
      //      Cli_Usart_Data.tx_counter = sprintf((char *)Cli_Usart_Data.tx_buffer, "Teste STM32 Cli\r\n");
      //      HAL_UART_Transmit(&Cli_USART_Handler, Cli_Usart_Data.tx_buffer, Cli_Usart_Data.tx_counter, 1000);
    }
  }
}

int consolePutchar(char c) {
  HAL_UART_Transmit(&Cli_USART_Handler, (uint8_t *)&c, 1, 10);
  return 0;
}
