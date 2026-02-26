/*********************************************************************
* All Rights Reserved.
* @file bsp_key_led.c
* @par dependencies
- bsp_key_led.h
* @author zhangqi
* @brief Provide the HAL APIs of key and corresponding opetions.
* processing flow:
* call directly.
* @version V1.0 2026-02-25
* @note 1 tab == 4 spaces!
********************************************************************/

//**************************** Includes ***************************//
//包含自己的寄存器.h文件一定是在最开始
#include "bsp_key_led.h"

//**************************** Includes ***************************//

//************************ Thread_FUNC ****************************//
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
  .name = "keyTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
//************************ Thread_FUNC ****************************//


//************************ Queue_Handler ****************************//
QueueHandle_t x_key_Queue;
//************************ Queue_Handler ****************************//

/**
  * @brief  Function implementing the keyTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartkeyTask */
key_status_t key_scan(key_press_status_t *key_value)
{
    uint32_t counter                    =               0;
    key_press_status_t key_status_value = KEY_NOT_PRESSED;
    while (counter < 1000)
    {
        //2.如果按键（PA0）的电平为低，则说明按键被按下了。
        //2.1如果按键被按下，则发送对应的消息队列
        if (HAL_GPIO_ReadPin (Key_GPIO_Port, Key_Pin)== GPIO_PIN_RESET)
        {
            key_status_value = KEY_PRESSED;
            *key_value = key_status_value;
            return KEY_OK;    
        }
        //2.2如果按键不被按下，则计数增加
        counter++;
    }
    *key_value = key_status_value;
    //始终没有按键被按下，返回超时
    return KEY_ERRORTIMEOUT;
}

/**
  * @brief  Function implementing the keyTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartkeyTask */
void StartkeyTask(void *argument)
{
    /* USER CODE BEGIN StartkeyTask */
    key_status_t key_ret          =          KEY_OK;
    key_press_status_t key_status = KEY_NOT_PRESSED;
    x_key_Queue = xQueueCreate( 10, sizeof( key_status_t ) );
    /* Infinite loop */
    for(;;)
    {
    key_ret = key_scan(&key_status);

    if(KEY_OK == key_ret)
    {
        if(KEY_PRESSED == key_status)
        {
            printf("KEY_PRESSED \r\n");
            if( x_key_Queue != 0 )
            {
                // available if necessary.
                if( pdPASS == xQueueSend( x_key_Queue,  &key_status, ( TickType_t ) 10 )  )
                {
                    printf("QueueSend successfully\r\n");
                }
                // Failed to post the message, even after 10 ticks.
                else if(pdPASS != xQueueSend( x_key_Queue,  &key_status, ( TickType_t ) 10 ))
                {
                    printf("QueueSend failed\r\n");
                }
            }
        }
    }
    if(KEY_OK != key_ret)
    {
        printf("KEY_NOT_PRESSED \r\n");
    }

    osDelay(100);
    }
    /* USER CODE END StartkeyTask */
}


