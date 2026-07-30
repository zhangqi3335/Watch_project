/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file bsp_adc.c
 *
 * @author 张奇
 *
 * @brief Board Support Package for ADC control.
 *
 * @version V1.0 <2026-06-04>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "bsp_uart_driver.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "queue.h"
#include "elog.h"
#include "mid_circular_buffer.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define BUFFER_A 0
#define BUFFER_B 1
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
#if 0 // AB buffer
uint8_t flag_AB = BUFFER_A;
#endif
static circular_buffer_t *gp_circular_buffer_irq_thread = NULL;
/* Private function prototypes ----------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
#if 0 // AB buffer
uint8_t g_buffer_A[1] = {0x00};
uint8_t g_buffer_B[1] = {0x00};
#endif
#if 1 // ring buffer
uint8_t g_data_buffer = 0;
#endif
extern QueueHandle_t queue_irq_rec_A;
extern UART_HandleTypeDef huart1;
/* Public functions ---------------------------------------------------------*/
/**
 * @brief  Function implementing the Task_uart_rec_A thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_uart_driver_func */
void uart_driver_func(void *argument)
{
    /* USER CODE BEGIN uart_driver_func */
    // alloc the ring buffer
    circular_buffer_t *p_circular_buffer = create_empty_circular_buffer();
    if (NULL == p_circular_buffer)
    {
        log_e("Failed to create circular buffer");
        return;
    }
    gp_circular_buffer_irq_thread = p_circular_buffer;
    if (0x00 == buffer_is_empty(p_circular_buffer))
    {
        log_i("Circular buffer is empty");
    }
    if (0x00 == buffer_is_full(p_circular_buffer))
    {
        log_i("Circular buffer is full");
    }
    // insert_data(p_circular_buffer, 0x01);
    // uint8_t data;
    // get_data(p_circular_buffer, &data);
    // log_i("Data retrieved from circular buffer: %d", data);

    // flag_AB = BUFFER_A;
    HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
    log_i("uart_driver_func is running");
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END uart_driver_func */
}


/**
 * @brief  Rx Transfer completed callbacks.
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
#if 0 // AB buffer
    log_d("HAL_UART_RxCpltCallback");
    long ret = 0;
    uint32_t send_data_to_rec_A = 0x02;
    ret = xQueueSendFromISR(queue_irq_rec_A, &send_data_to_rec_A, NULL);
    log_d("xQueueSendFromISR ret = %d", ret);
    if(BUFFER_A == flag_AB)
    {
        flag_AB = BUFFER_B;
        HAL_UART_Receive_IT(&huart1, g_buffer_B, 1);
        log_i("Data g_buffer_A[0] = %d", g_buffer_A[0]);
    }
    else if(BUFFER_B == flag_AB)
    {
        flag_AB = BUFFER_A;
        HAL_UART_Receive_IT(&huart1, g_buffer_A, 1);
        log_i("Data g_buffer_B[0] = %d", g_buffer_B[0]);
    }
#endif
#if 1 // ring buffer
    if (NULL == gp_circular_buffer_irq_thread)
    {
        log_e("gp_circular_buffer_irq_thread is NULL");
        return;
    }
    uint8_t ret_1 = 0;
    ret_1 = insert_data(gp_circular_buffer_irq_thread, g_data_buffer);
    if (0x00 == ret_1)
    {
        uint8_t data_temp = 0;
        if (get_data(gp_circular_buffer_irq_thread, &data_temp) == 0x00)
        {
            log_i("Data retrieved from circular buffer: %d", data_temp);
        }
        else
        {
            log_e("Failed to retrieve data from circular buffer");
        }
        HAL_StatusTypeDef ret_2 = HAL_OK;
        ret_2 = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
        if (HAL_OK == ret_2)
        {
            log_i("HAL_UART_Receive_IT ret_2 = HAL_OK");
        }
        else
        {
            log_e("Failed to start HAL_UART_Receive_IT");
        }
    }
#endif
    /* NOTE: This function should not be modified, when the callback is needed,
             the HAL_UART_RxCpltCallback could be implemented in the user file
     */
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->ErrorCode & HAL_UART_ERROR_ORE)
    {
        // 发生过载，说明字节在没重新启动接收前就到了
        // 可以在这里打个标记或翻转一个GPIO
			log_a("ORE Error");
    }
}
/* Private functions --------------------------------------------------------*/
