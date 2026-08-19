/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file debug.c
 *
 * @author 张奇
 *
 * @brief  Implementation for Debug task.
 *
 * @version V1.0 <2026-08-12>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "debug.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
void app_elog_init(void)
{
	elog_init();
	/* set EasyLogger log format */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	/* start EasyLogger */
	elog_start();
}
/* Private functions --------------------------------------------------------*/
