/********************************************************************
* All Rights Reserved.
* @file bsp_key_irq.h
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

#ifndef _BSP_KEY_IRQ_H_
#define _BSP_KEY_IRQ_H_
//**************************** Includes ***************************//
#include <stdint.h>                         //the compilation
#include <stdio.h>

#include "main.h"                           //Core / OS layer
#include "cmsis_os.h"

#include "queue.h"                          //specific file of operation
#include "tim.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "usart.h"


//**************************** Includes ***************************//


//**************************** Defines ****************************//
#define Delay_10MS 1
#define Delay_500MS 50


#define KEY_CALLBACK             void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//************************ Thread_FUNC ****************************//
extern osThreadId_t keyTaskHandle;
extern const osThreadAttr_t keyTask_attributes;
//************************ Thread_FUNC ****************************//


//************************ Queue_Handler ****************************//
extern QueueHandle_t x_key_Queue;
extern QueueHandle_t x_key_irq_Queue;
//************************ Queue_Handler ****************************//


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
    KEY_NOT_PRESSED     = 0,    /* Operation not completed successful*/
    KEY_SHORT_PRESSED   = 1,    /* Operation short press             */
    KEY_LONG_PRESSED    = 2,    /* Operation long press              */
} key_press_status_t;

typedef enum
{
    KEY_STATE_IDLE       = 0,    /* Operation completed successful    */
    KEY_STATE_PRESS      = 1,	 /* Operation completed successful    */
    KEY_STATE_CONFRIM	 = 2     /* Operation completed successful    */
} key_state_t;

typedef struct
{   
    key_state_t                  state;
    key_press_status_t      press_type;
    uint32_t             press_counter;
    GPIO_TypeDef*            gpio_port;      
    uint16_t                  gpio_pin;       
}key_device_t;

typedef enum{
	FALLING = 0,
	RISING  = 1,
}key_trigger_edge_t;

typedef struct 
{
    uint32_t       trigger_tick;      /* 中断触发时的系统systick值               */
    key_trigger_edge_t edge_type;     /* 按键事件：0-按下(下降沿) 1-松开(上升沿)  */
}key_interrupt_msg_t;


//**************************** Defines ****************************//           

//*************************** Declaring ***************************//

void StartkeyTask(void *argument);
key_status_t key_scan(key_device_t* g_key_dev,key_press_status_t* press_type);
//*************************** Declaring ***************************//
#endif

