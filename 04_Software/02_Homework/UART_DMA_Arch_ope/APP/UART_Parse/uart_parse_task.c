/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file uart_parse_task.c
 *
 * @author 张奇
 *
 * @brief UART parse task.
 *
 * @version V1.0 <2026-07-24>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "uart_parse_task.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "queue.h"
#include "elog.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
QueueHandle_t queue_irq_rec_A = NULL;
/* Private function prototypes ----------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
/**
 * @brief  Function implementing the Task_uart_rec_A thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_uart_rec_A_func */
void uart_rec_A_func(void *argument)
{
  /* USER CODE BEGIN uart_rec_A_func */
  uint32_t rec_data = 0;
  log_i("uart_rec_A_func is running");
  queue_irq_rec_A = NULL;
  queue_irq_rec_A = xQueueCreate(1, 4);
  if (NULL == queue_irq_rec_A)
  {
    log_i("queue_irq_rec_A Init failed");
  }
  else
  {
    log_i("queue_irq_rec_A Init Success");
    log_i("queue_irq_rec_A = [%x}", queue_irq_rec_A);
  }
  /* Infinite loop */
  for (;;)
  {
    xQueueReceive(queue_irq_rec_A, &rec_data, portMAX_DELAY);
    log_i("Received data: %d", rec_data);
    osDelay(1);
  }
  /* USER CODE END uart_rec_A_func */
}

/* Private functions --------------------------------------------------------*/
