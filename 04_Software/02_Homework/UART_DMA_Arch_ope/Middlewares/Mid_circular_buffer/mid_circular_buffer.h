/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file mid_circular_buffer.h
 *
 * @author 张奇
 *
 * @brief Middeleware for circular buffer.
 *
 * @version V1.0 <2026-07-29>
 *
 *****************************************************************************/
#ifndef __MID_CIRCULAR_BUFFER_H__
#define __MID_CIRCULAR_BUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Include ----------------------------------------------------------------- */
#include <stdint.h>
/* Exported Defines ---------------------------------------------------------*/
#define CIRCULAR_BUFFER_SIZE 100
/* Exported typedef ---------------------------------------------------------*/
typedef uint8_t data_type_t;
typedef struct
{
    data_type_t data[CIRCULAR_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
}circular_buffer_t;
/* Exported FunctionPrototypes  -------------------------------------------- */
circular_buffer_t * create_empty_circular_buffer(void);
uint8_t buffer_is_empty  (circular_buffer_t * p_buffer);
uint8_t buffer_is_full   (circular_buffer_t * p_buffer);
uint8_t insert_data      (circular_buffer_t * p_buffer, data_type_t data) ;
uint8_t get_data         (circular_buffer_t * p_buffer, data_type_t * data);
/* Exported functions -------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif //__MID_CIRCULAR_BUFFER_H__
