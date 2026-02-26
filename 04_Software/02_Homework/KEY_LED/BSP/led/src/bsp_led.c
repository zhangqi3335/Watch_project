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
QueueHandle_t x_led_Queue;
//************************ Queue_Handler ****************************//

//**************************** Defines ****************************//

//**************************** Defines ****************************//

/**
* @brief  Sets or clears the selected data port bit.
*
* @note   This function 
*
* @param[in] led_operation_t
* @return led_hander_status_t :Status of the function
**/
led_status_t led_on_off(led_operation_t led_operation)
{
	led_status_t led_status_ret = LED_OK;
	if(LED_ON == led_operation)
	{
		//1.Write LED on
		HAL_GPIO_WritePin(Led_GPIO_Port,Led_Pin,GPIO_PIN_SET);
	}
    else if(LED_OFF == led_operation){
        //2.Write LED off
        HAL_GPIO_WritePin(Led_GPIO_Port,Led_Pin,GPIO_PIN_RESET);
    }
    else if(LED_TOGGLE == led_operation){
        //3.Write LED toggle
        HAL_GPIO_TogglePin(Led_GPIO_Port,Led_Pin);
    }
    else
    {
        led_status_ret = LED_ERROR;
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
  led_status_t led_ret              =          LED_OK;
  led_operation_t led_ops_status    =          LED_ON;
  x_led_Queue = xQueueCreate( 10, sizeof( led_operation_t ) );
  /* Infinite loop */
  for(;;)
  {
	if(LED_OK == led_ret)
    {
        printf("led start \r\n");
        if( x_led_Queue != 0 )
        {
            // available if necessary.
            if( pdPASS == xQueueReceive( x_led_Queue,  &led_ops_status, ( TickType_t ) 10 )  )
            {
                printf("ledQueueSend successfully\r\n");
                led_on_off(led_ops_status);
            }
            // Failed to post the message, even after 10 ticks.
            else if(pdPASS != xQueueReceive( x_led_Queue,  &led_ops_status, ( TickType_t ) 10 ))
            {
                printf("ledQueueSend failed\r\n");
            }
        }
    }
    else if(LED_OK != led_ret)
    {
      printf("led_NOT_PRESSED \r\n");
    }
    osDelay(100);
  }
  /* USER CODE END StartledTask */
}


