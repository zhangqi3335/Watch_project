/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file bsp_adc.h
 *
 * @author 张奇
 *
 * @brief Board Support Package for ADC control.
 *
 * @version V1.0 <2026-05-07>
 *
 *****************************************************************************/
#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Include ----------------------------------------------------------------- */
#include "stm32f4xx_hal.h"
    /* Exported typedef ---------------------------------------------------------*/
    /* Exported Defines ---------------------------------------------------------*/
    /* Exported variables -------------------------------------------------------*/
    extern ADC_HandleTypeDef hadc1;
    /* Exported functions -------------------------------------------------------*/
    /**
     * @brief Create ADC tasks for FreeRTOS.
     * @param[in] None
     * @retval None
     */
    void Task_ADC_Create(void);

    /**
     * @brief Initialize the ADC DMA and buffers.
     * @param[in] None
     * @retval None
     */
    void BSP_ADC_Init(void);
#ifdef __cplusplus
}
#endif

#endif //__BSP_ADC_H__
