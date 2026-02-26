/********************************************************************
* All Rights Reserved.
* @file bsp_key_led.h
* @par dependencies
- stdio.h
- stdint.h
* @author zhangqi
* @brief Provide the HAL APIs of key and corresponding opetions.
* processing flow:
* call directly.
* @version V1.0 2026-02-25
* @note 1 tab == 4 spaces!
********************************************************************/

#ifndef _BSP_KEY_LED_H_
#define _BSP_KEY_LED_H_
//**************************** Includes ***************************//
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

//编译器提供的通用库包含部分
#include <stdint.h>
#include <stdio.h>
//**************************** Includes ***************************//


//**************************** Defines ****************************//

/* 函数返回状态枚举 */
typedef enum
{
    KEY_OK              = 0,        /* Operation completed successful*/
    KEY_ERROR           = 1,        /* Run-time error without success*/
    KEY_ERRORTIMEOUT    = 2,        /* Operation failed with timeout */
    KEY_ERRORRESOURCE   = 3,        /* Resource not available.       */
    KEY_ERRORPARAMETER  = 4,        /* Parameter error.              */
    KEY_ERRORNOMEMORY   = 5,        /* Out of memory.                */
    KEY_ERRORISR        = 6,        /* Not allowed in ISR context    */
    KEY_RESERVED        = 0x7FFFFFFF/* Reserved                      */
} key_status_t;

typedef enum
{
    KEY_PRESSED         = 0,    /* Operation completed successful    */
    KEY_NOT_PRESSED     = 1,    /* Operation not completed successful*/                  
} key_press_status_t;

//**************************** Defines ****************************//


//*************************** Declaring ***************************//
key_status_t key_scan(key_press_status_t *key_value);
//*************************** Declaring ***************************//
#endif

