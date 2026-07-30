/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file mid_circular_buffer.c
 *
 * @author 张奇
 *
 * @brief Middleware for circular buffer.
 *
 * @version V1.0 <2026-07-30>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "mid_circular_buffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
/* Public functions ---------------------------------------------------------*/
/**
 * @brief Create a empty circular buffer object
 *
 * @return circular_buffer_t*
 */
circular_buffer_t *create_empty_circular_buffer(void)
{
    // 分配整个结构体的内存
    circular_buffer_t *p_buffer = (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
    
    if (NULL != p_buffer)
    {
        memset(p_buffer->data, 0, CIRCULAR_BUFFER_SIZE); 
        p_buffer->head = 0;
        p_buffer->tail = 0;
    }
    
    return p_buffer;
}

/**
 * @brief Check if the circular buffer is empty
 *
 * @param[in] p_buffer Pointer to the circular buffer
 * @return  0xFF if the buffer is NULL,
 *          0x00 if the buffer is empty,
 *          0x01 if the buffer is not empty
 */
uint8_t buffer_is_empty(circular_buffer_t *p_buffer)
{
    if (NULL == p_buffer)
    {
        return 0xFF; // Buffer is empty if pointer is NULL
    }
    if (p_buffer->head == p_buffer->tail)
    {
        return 0x00; // Buffer is empty
    }
    else
    {
        return 0x01; // Buffer is not empty
    }
}

/**
 * @brief Check if the circular buffer is full
 *
 * @param[in] p_buffer Pointer to the circular buffer
 * @return  0xFF if the buffer is NULL,
 *          0x00 if the buffer is full,
 *          0x01 if the buffer is not full
 */
uint8_t buffer_is_full(circular_buffer_t *p_buffer)
{
    if (NULL == p_buffer)
    {
        return 0xFF; // Buffer is full if pointer is NULL
    }
    if ((p_buffer->head + 1) % CIRCULAR_BUFFER_SIZE == (p_buffer->tail) % CIRCULAR_BUFFER_SIZE)
    {
        return 0x00; // Buffer is full
    }
    else
    {
        return 0x01; // Buffer is not full
    }
}

/**
 * @brief Insert data into the circular buffer
 *
 * @param[in] p_buffer Pointer to the circular buffer
 * @param data The data to be inserted
 * @return 0xFF if the buffer is NULL,
 *         0xFE if the buffer is full,
 *         0x00 if the data is inserted successfully
 */
uint8_t insert_data(circular_buffer_t *p_buffer, data_type_t data)
{
    if (NULL == p_buffer)
    {
        return 0xFF; // Buffer is NULL
    }
    if (buffer_is_full(p_buffer) == 0x00)
    {
        return 0xFE; // Buffer is full
    }
    p_buffer->data[p_buffer->head] = data;
    p_buffer->head = (p_buffer->head + 1) % CIRCULAR_BUFFER_SIZE;
    return 0x00; // Data inserted successfully
}

/**
 * @brief Get data from the circular buffer
 *
 * @param[in] p_buffer Pointer to the circular buffer
 * @param[out] data The data to be retrieved
 * @return 0xFF if the buffer is NULL,
 *         0xFE if the buffer is empty,
 *         0x00 if the data is retrieved successfully
 */
uint8_t get_data(circular_buffer_t *p_buffer, data_type_t *data)
{
    if (NULL == p_buffer || NULL == data)
    {
        return 0xFF; // Buffer or data pointer is NULL
    }
    if (buffer_is_empty(p_buffer) == 0x00)
    {
        return 0xFE; // Buffer is empty
    }
    *data = p_buffer->data[p_buffer->tail];
    p_buffer->tail = (p_buffer->tail + 1) % CIRCULAR_BUFFER_SIZE;
    return 0x00; // Data retrieved successfully
}
/* Private functions --------------------------------------------------------*/
