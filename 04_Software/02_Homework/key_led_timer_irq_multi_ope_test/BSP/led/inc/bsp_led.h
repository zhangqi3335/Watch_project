/********************************************************************
* All Rights Reserved.
* @file bsp_led.h
* @par dependencies
- stdio.h
- stdint.h
* @author zhangqi
* @brief Provide the HAL APIs of led and corresponding opetions.
* processing flow:
* call directly.
* @version V1.0 2026-02-25
* @note 1 tab == 4 spaces!
********************************************************************/

#ifndef _BSP_LED_H_
#define _BSP_LED_H_
//**************************** Includes ***************************//
#include <stdint.h>                         //the compilation
#include <stdio.h>

#include "main.h"                           //Core / OS layer
#include "cmsis_os.h"

#include "queue.h"                          //specific file of operation
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

//**************************** Includes ***************************//


//**************************** Defines ****************************//
//************************ Thread_FUNC ****************************//
extern osThreadId_t ledTaskHandle;
extern const osThreadAttr_t ledTask_attributes;
//************************ Thread_FUNC ****************************//


//************************ Queue_Handler ****************************//
extern QueueHandle_t x_led_Queue;
//************************ Queue_Handler ****************************//



/* function return status */
typedef enum
{
    LED_OK              = 0,        /* Operation completed successful*/
    LED_ERROR           = 1,        /* Run-time error without success*/
    LED_ERRORTIMEOUT    = 2,        /* Operation failed with timeout */
    LED_ERRORRESOURCE   = 3,        /* Resource not available.       */
    LED_ERRORPARAMETER  = 4,        /* Parameter error.              */
    LED_ERRORNOMEMORY   = 5,        /* Out of memory.                */
    LED_ERRORISR        = 6,        /* Not allowed in ISR context    */
    LED_RESERVED        = 0x7FFFFFFF/* Reserved                      */
} led_status_t;

typedef enum
{
    LED_ON              = 0,        /* Operation completed successful*/
    LED_OFF             = 1,        /* Operation completed successful*/
    LED_TOGGLE          = 2,        /* Operation completed successful*/
    LED_FLASH           = 3,        /* Operation completed successful*/
} led_operation_t;

typedef struct
{
    uint8_t led_flash_times;   // 闪烁次数
    uint8_t led_pwm_running;   // PWM控制状态
    uint8_t led_current_times; // 当前闪烁次数
} led_control_t;
//**************************** Defines ****************************//


//*************************** Declaring ***************************//
void StartledTask(void *argument);
led_status_t led_on_off(led_operation_t led_operation);

extern led_control_t g_led_ctrl;
//*************************** Declaring ***************************//
#endif

