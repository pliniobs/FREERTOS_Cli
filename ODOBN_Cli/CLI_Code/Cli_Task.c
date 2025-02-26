#include "Cli_Task.h"

#include <stdio.h>

#include "Cli_Uart.h"
#include "FreeRTOS.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "task.h"

// Task Handle
TaskHandle_t xTask_Cli_Handler;

/* Function Prototype */
void VTask_Cli_Manager(void *pvParameters);
void VTask_Cli_Init_Usart(void);

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
  GPIO_InitTypeDef GPIO_Init_Struct;

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_Init_Struct.Pin = GPIO_PIN_5;
  GPIO_Init_Struct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_Init_Struct);

  while (1) {
    if (xTaskNotifyWait(0, 0, &Notification_Value, 1000) == pdPASS) {
      Cli_Usart_Data.tx_counter = snprintf((char *)Cli_Usart_Data.tx_buffer, RX_TX_BUFFER_SIZE, "Received %d: %s\r\n", Cli_Usart_Data.rx_counter, Cli_Usart_Data.rx_buffer);
      HAL_UART_Transmit(&Cli_USART_Handler, Cli_Usart_Data.tx_buffer, Cli_Usart_Data.tx_counter, 1000);

      /*
       * Prepare the usart to receive a new command
       * */
      Cli_Usart_Data.rx_counter = 0;
      HAL_UART_Receive_IT(&Cli_USART_Handler, &Cli_Usart_Data.Input_char, 1);  // Prepare the UART to receive the next character
    } else {
      Cli_Usart_Data.tx_counter = sprintf((char *)Cli_Usart_Data.tx_buffer, "Teste STM32 Cli\r\n");
      HAL_UART_Transmit(&Cli_USART_Handler, Cli_Usart_Data.tx_buffer, Cli_Usart_Data.tx_counter, 1000);
    }

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
}
