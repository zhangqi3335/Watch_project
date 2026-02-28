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
  .priority = (osPriority_t) osPriorityHigh,
};
//************************ Thread_FUNC ****************************//

//************************ Queue_Handler ****************************//
QueueHandle_t x_key_Queue  = NULL;
//************************ Queue_Handler ****************************//

/* USER CODE BEGIN PV */
key_device_t g_key_dev=
{
    .state = KEY_STATE_IDLE,
    .press_type = KEY_NOT_PRESSED,
    .press_counter = 0,
    .gpio_port = Key_GPIO_Port,
    .gpio_pin = Key_Pin
};
/* USER CODE END PV */


/**
  * @brief  Function implementing the keyTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartkeyTask */
key_status_t key_scan(key_device_t* g_key_dev,key_press_status_t* press_type)
{
    printf("【调试】进入检测\r\n");
    key_status_t ret = KEY_ERRORTIMEOUT;
    *press_type = KEY_NOT_PRESSED;
    GPIO_PinState bitstatus = HAL_GPIO_ReadPin(g_key_dev->gpio_port,g_key_dev->gpio_pin);

    if(g_key_dev == NULL || press_type == NULL)
    {
        ret = KEY_ERRORPARAMETER;
        return ret;
    }

    switch (g_key_dev->state)
    {
    case KEY_STATE_IDLE:
        printf("【调试】进入检测IDLE\r\n");
        if(GPIO_PIN_RESET == bitstatus)
        {
            g_key_dev->press_counter++;
            if(g_key_dev->press_counter > Delay_10MS)
            {
                g_key_dev->press_counter = 0;
                g_key_dev->state = KEY_STATE_PRESS;
            }
        }
        else
        {
            g_key_dev->press_counter = 0;
        }
        break;
    
    case KEY_STATE_PRESS:
    
        printf("【调试】进入检测press\r\n");
        printf("【调试】PRESS态计数：%d\r\n", g_key_dev->press_counter);
        if(GPIO_PIN_RESET == bitstatus)
        {
            g_key_dev->press_counter++;
            if(g_key_dev->press_counter > Delay_500MS)
            {
                g_key_dev->state = KEY_STATE_CONFRIM;
                g_key_dev->press_type = KEY_LONG_PRESSED;
            }
        }
        else // 按键松开
        {
            if(g_key_dev->press_counter <= Delay_500MS && g_key_dev->press_counter > Delay_10MS)
            {
                g_key_dev->state = KEY_STATE_CONFRIM;
                g_key_dev->press_type = KEY_SHORT_PRESSED;
            }
            else
            {
                // 无效按键（按的时间太短）
                g_key_dev->state = KEY_STATE_IDLE;
                g_key_dev->press_counter = 0;
            }
        }
        break;

    case KEY_STATE_CONFRIM:
        printf("【调试】进入检测Confirm\r\n");
        *press_type = g_key_dev->press_type;
        ret = KEY_OK;
        g_key_dev->state = KEY_STATE_IDLE;
        g_key_dev->press_counter = 0;
        g_key_dev->press_type = KEY_NOT_PRESSED;
        break;        
    default:
        g_key_dev->state = KEY_STATE_IDLE; // 异常状态重置
        ret = KEY_ERROR;
        break;
    }
    return ret;
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
    key_press_status_t key_press_type = KEY_NOT_PRESSED;
    x_key_Queue = xQueueCreate( 10, sizeof( key_press_status_t ) );
    /* Infinite loop */
    for(;;)
    {
			key_ret = key_scan(&g_key_dev,&key_press_type);

			if(KEY_OK == key_ret && key_press_type!=KEY_NOT_PRESSED)
			{
					
					if(xQueueSend(x_key_Queue, &key_press_type, (TickType_t)10) == pdPASS)
					{
							printf("Send %s to queue success!\r\n", 
											key_press_type == KEY_SHORT_PRESSED ? "SHORT_PRESS" : "LONG_PRESS");
					}
					else
					{
							printf("Queue send failed!\r\n");
					}
			}
			if(KEY_OK != key_ret)
			{
					//printf("KEY_NOT_PRESSED \r\n");
			}

			osDelay(10);
    }
    /* USER CODE END StartkeyTask */
}


