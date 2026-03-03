/*********************************************************************
* All Rights Reserved.
* @file bsp_key_irq.c
* @par dependencies
- bsp_key_irq.h
* @author zhangqi
* @brief Provide the HAL APIs of key and corresponding opetions.
* processing flow:
* call directly.
* @version V1.0 2026-02-25
* @note 1 tab == 4 spaces!
********************************************************************/


//**************************** Includes ***************************//
//包含自己的寄存器.h文件一定是在最开始
#include "bsp_key_irq.h"

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
QueueHandle_t x_key_Queue  = NULL;
QueueHandle_t x_key_irq_Queue = NULL;
//************************ Queue_Handler ****************************//

/* USER CODE BEGIN PV */

#define KEY_SHORT_THRESHOLD   50     // 50ms
#define KEY_LONG_THRESHOLD    500    // 500ms
#define KEY_JITTER_THRESHOLD  20     // 20ms

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
    key_interrupt_msg_t msg;
    uint32_t t1 = 0;
    uint32_t t2 = 0;
    uint32_t delta = 0;

    x_key_Queue = xQueueCreate(10, sizeof(key_press_status_t));
    x_key_irq_Queue = xQueueCreate(10, sizeof(key_interrupt_msg_t));
    if(x_key_Queue == NULL || x_key_irq_Queue == NULL)
    {
        printf("Key queue create failed\r\n");
        vTaskDelete(NULL);
    }
	
    /* Infinite loop */
    for(;;)
    {
		 if(xQueueReceive(x_key_irq_Queue, &msg, 10) == pdTRUE)
        {
            printf("IRQ received\r\n");
            if(msg.edge_type == FALLING)
            {
                t1 = msg.trigger_tick;
            }
            else if(msg.edge_type == RISING)
            {
                t2 = msg.trigger_tick;
                delta = t2 - t1;

                key_press_status_t result = KEY_NOT_PRESSED;

                if(delta < KEY_JITTER_THRESHOLD)
                {
                    // 抖动，忽略
                    printf("bad press\r\n");
                    continue;
                }
                else if(delta < KEY_LONG_THRESHOLD)
                {
                    result = KEY_SHORT_PRESSED;
                    printf("short press\r\n");
                }
                else
                {
                    result = KEY_LONG_PRESSED;
                    printf("long press\r\n");
                }

                if(xQueueSend(x_key_Queue, &result, 0) == pdTRUE)
                {
                printf("send ok\r\n");
                }
                else
                {
                printf("send fail\r\n");
                }
            }
        }
        osDelay(10);
    }
    /* USER CODE END StartkeyTask */
}


KEY_CALLBACK{
    if(GPIO_Pin == Key_Pin)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        key_interrupt_msg_t msg;

        msg.trigger_tick = HAL_GetTick();

        if(HAL_GPIO_ReadPin(Key_GPIO_Port, Key_Pin) == GPIO_PIN_RESET)
        {
            msg.edge_type = FALLING;  // 按下
        }
        else
        {
            msg.edge_type = RISING;   // 松开
        }

        if(x_key_irq_Queue != NULL)
        {
            xQueueSendFromISR(x_key_irq_Queue, &msg, &xHigherPriorityTaskWoken);
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

