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

