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
 * @version V1.0 <2026-06-08>
 *
 *****************************************************************************/
#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Include ----------------------------------------------------------------- */
/* Exported typedef ---------------------------------------------------------*/
/* Exported Defines ---------------------------------------------------------*/
/* Exported functions -------------------------------------------------------*/
void BSP_ADC_Init(void);
void BSP_ADC_Start(void);
#ifdef __cplusplus
}
#endif

#endif //__BSP_ADC_H__
