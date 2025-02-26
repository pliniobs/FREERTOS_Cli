/*
 * Cli_Task.h
 *
 *  Created on: Feb 21, 2025
 *      Author: plinio
 */

#ifndef CLI_TASK_H_
#define CLI_TASK_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "task.h"

extern TaskHandle_t xTask_Cli_Handler;

void Init_Cli_Task(void);

#endif /* CLI_TASK_H_ */
