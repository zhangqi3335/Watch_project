/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define LOG_TAG "Freertos"
#include "elog.h"
#include "bsp_uart_driver.h"
#include "uart_parse_task.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define LOG_TAG "FREERTOS"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Definitions for bsp_uart_driver */
osThreadId_t Task_bsp_uart_driverHandle;
const osThreadAttr_t Task_bsp_uart_driver_attributes = {
    .name = "Task_bsp_uart_driver",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* USER CODE END Variables */
/* Definitions for Task_uart_rec_A */
osThreadId_t Task_uart_rec_AHandle;
const osThreadAttr_t Task_uart_rec_A_attributes = {
    .name = "Task_uart_rec_A",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void uart_driver_func(void *argument);
/* USER CODE END FunctionPrototypes */

void uart_rec_A_func(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_uart_rec_A */
  Task_uart_rec_AHandle = osThreadNew(uart_rec_A_func, NULL, &Task_uart_rec_A_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Task_bsp_uart_driverHandle = osThreadNew(uart_driver_func, NULL, &Task_bsp_uart_driver_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_uart_rec_A_func */
/**
 * @brief  Function implementing the Task_uart_rec_A thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_uart_rec_A_func */
// void uart_rec_A_func(void *argument)
// {
//   /* USER CODE BEGIN uart_rec_A_func */
//   log_i("uart_rec_A_func is running");
//   /* Infinite loop */
//   for (;;)
//   {

//     osDelay(1);
//   }
//   /* USER CODE END uart_rec_A_func */
// }

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
