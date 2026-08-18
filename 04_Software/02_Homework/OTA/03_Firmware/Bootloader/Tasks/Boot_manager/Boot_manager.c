/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file Boot_manager.c
 *
 * @author 张奇
 *
 * @brief  Implementation for Boot manager task.
 *
 * @version V1.0 <2026-08-12>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "Boot_manager.h"

#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "elog.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
static pFunction Jump_To_Application;
static uint32_t JumpAddress; // PC
/* Private function prototypes ----------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
void Jump_APP(void)
{
    if (((*(__IO uint32_t *)APP_FLASH_ADDR) & 0x2FFE0000) == 0x20000000)
    {
			log_i("Boot jump");
        __disable_irq();
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
				RCC_DeInit();
        // 找到PC的地址在APP的起始地址+4的地方
        JumpAddress = *(__IO uint32_t *)(APP_FLASH_ADDR + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        // 设置MSP在APP的起始地址
        __set_MSP(*(__IO uint32_t *)APP_FLASH_ADDR);
        // 跳转到APP
        Jump_To_Application();
    }
}
/* Private functions --------------------------------------------------------*/

