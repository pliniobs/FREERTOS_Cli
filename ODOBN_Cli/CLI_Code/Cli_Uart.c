#include "Cli_Uart.h"

#include "Cli_Task.h"
#include "main.h"
#include "stm32l4xx_hal.h"

UART_HandleTypeDef Cli_USART_Handler;

Cli_USART_Data_t Cli_Usart_Data = {
    .rx_counter = 0,
    .tx_counter = 0,
};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;

  if (huart->Instance == USART2) {
    if (Cli_Usart_Data.rx_counter >= RX_TX_BUFFER_SIZE)  // Check the buffer overflow
      Cli_Usart_Data.rx_counter = 0;

    if ((Cli_Usart_Data.Input_char == '\r' || Cli_Usart_Data.Input_char == '\n') && (Cli_Usart_Data.rx_counter > 0)) {
      Cli_Usart_Data.rx_buffer[Cli_Usart_Data.rx_counter] = 0;  // Place null terminator character
      // Inform the task the new incoming command.
      vTaskNotifyGiveFromISR(xTask_Cli_Handler,        /* The handle of the task to which the notification is being sent. The handle was saved when the task was created. */
                             &xHigherPriorityTaskWoken /* xHigherPriorityTaskWoken is used in the usual way. */
      );
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
      /*
       * Append data to the buffer and increment the counter
       */
      Cli_Usart_Data.rx_buffer[Cli_Usart_Data.rx_counter++] = Cli_Usart_Data.Input_char;  // Insert the char in the input buffer
      HAL_UART_Receive_IT(&Cli_USART_Handler, &Cli_Usart_Data.Input_char, 1);             // Prepare the UART to receive the next character
    }
  }
}

void LPUART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&Cli_USART_Handler);
}

void Cli_Uart_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }

  /* LPUART1 clock enable */
  __HAL_RCC_LPUART1_CLK_ENABLE();

  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  /**LPUART1 GPIO Configuration
  PG7     ------> LPUART1_TX
  PG8     ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* LPUART1 interrupt Init */
  HAL_NVIC_SetPriority(LPUART1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);

  Cli_USART_Handler.Instance = LPUART1;
  Cli_USART_Handler.Init.BaudRate = CLI_UART_BAUDRATE;
  Cli_USART_Handler.Init.WordLength = UART_WORDLENGTH_8B;
  Cli_USART_Handler.Init.StopBits = UART_STOPBITS_1;
  Cli_USART_Handler.Init.Parity = UART_PARITY_NONE;
  Cli_USART_Handler.Init.Mode = UART_MODE_TX_RX;
  Cli_USART_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  Cli_USART_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  Cli_USART_Handler.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  Cli_USART_Handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  Cli_USART_Handler.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&Cli_USART_Handler) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&Cli_USART_Handler, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&Cli_USART_Handler, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&Cli_USART_Handler) != HAL_OK) {
    Error_Handler();
  }

  HAL_UART_Receive_IT(&Cli_USART_Handler, &Cli_Usart_Data.Input_char, 1);  // Prepare the UART to receive the character
}

void Cli_Uart_Deinit(void) {
  /* Peripheral clock disable */
  __HAL_RCC_LPUART1_CLK_DISABLE();
  /**LPUART1 GPIO Configuration
  PG7     ------> LPUART1_TX
  PG8     ------> LPUART1_RX
  */
  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7 | GPIO_PIN_8);
  /* LPUART1 interrupt Deinit */
  HAL_NVIC_DisableIRQ(LPUART1_IRQn);
}
