/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file Boot_manager.h
 *
 * @author 张奇
 *
 * @brief  Implementation for Boot manager task.
 *
 * @version V1.0 <2026-08-12>
 *
 *****************************************************************************/
#ifndef __BOOT_MANAGER_H__
#define __BOOT_MANAGER_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Include ----------------------------------------------------------------- */
/* Exported typedef ---------------------------------------------------------*/
typedef void(*pFunction)(void);
/* Exported Defines ---------------------------------------------------------*/
#define APP_FLASH_ADDR 0x08008000U
/* Exported functions -------------------------------------------------------*/
void Jump_APP(void);
#ifdef __cplusplus
}
#endif

#endif //__BOOT_MANAGER_H__
