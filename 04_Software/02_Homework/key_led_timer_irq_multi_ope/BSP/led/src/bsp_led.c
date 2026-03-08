/*********************************************************************
* All Rights Reserved.
* @file bsp_led.c
* @par dependencies
- bsp_led.h
* @author zhangqi
* @brief Provide the HAL APIs of led and corresponding opetions.
* processing flow:
* call directly.
* @version V1.0 2026-02-25
* @note 1 tab == 4 spaces!
********************************************************************/

//**************************** Includes ***************************//
#include "bsp_led.h"

//**************************** Includes ***************************//

//************************ Thread_FUNC ****************************//
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
//************************ Thread_FUNC ****************************//

//************************ Queue_Handler ****************************//
QueueHandle_t x_led_Queue  = NULL;
//************************ Queue_Handler ****************************//

//**************************** Defines ****************************//
led_control_t g_led_ctrl =
{
    .led_flash_times = 0,
    .led_pwm_running = 0,
    .led_current_times = 0
};
//**************************** Defines ****************************//

/**
* @brief  Sets or clears the selected data port bit.
*
* @note   This function 
*
* @param[in] led_operation_t: LED操作类型（ON/OFF/TOGGLE/FLASH）
* @return led_status_t :Status of the function
**/
led_status_t led_on_off(led_operation_t led_operation)
{
    led_status_t led_status_ret = LED_OK;
    
    // 参数合法性检查（避免非法参数）
    if(led_operation < LED_OFF || led_operation > LED_FLASH)
    {
        return LED_ERRORPARAMETER;
    }

    switch(led_operation)
    {
        case LED_ON:
            // 点亮LED（根据硬件极性调整SET/RESET）
            HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
            break;
            
        case LED_OFF:
            // 关闭LED
            HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
            break;
            
        case LED_TOGGLE:
            // 翻转LED状态
            HAL_GPIO_TogglePin(Led_GPIO_Port, Led_Pin);
            break;
            
        case LED_FLASH:
            // 闪烁3次（每次亮灭250ms）
            for(uint8_t i = 0; i < 3; i++)
            {
                HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
                osDelay(250);
                HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
                osDelay(250);
            }
            break;
            
        default:
            led_status_ret = LED_ERROR;
            break;
    }
    return led_status_ret;
}


/**
  * @brief  Function implementing the ledTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartledTask */
void StartledTask(void *argument)
{
  /* USER CODE BEGIN StartledTask */
    uint8_t received_flash_times      =              0;
    x_led_Queue = xQueueCreate( 10, sizeof( uint8_t) );
  /* Infinite loop */
    for(;;)
    {
        if( x_led_Queue != 0 )
        {
            // available if necessary.
            if( pdPASS == xQueueReceive( x_led_Queue,  &received_flash_times, ( TickType_t ) 10 )  )
            {
                if (0 == g_led_ctrl.led_pwm_running)
                {
                    g_led_ctrl.led_flash_times = received_flash_times;
                    g_led_ctrl.led_current_times = 0;
                    g_led_ctrl.led_pwm_running = 1;

                    HAL_TIM_Base_Start_IT(&htim2); // 启动定时器中断
                    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // 启动PWM输出

                    printf("ledQueueSend successfully\r\n");
                }
                else    
                {
                    printf("LED is busy\r\n");
                }
            }
        }
        osDelay(10);
    }
    /* USER CODE END StartledTask */
}


