#ifndef __UART_PARSE_TASK_H__
#define __UART_PARSE_TASK_H__

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "queue.h"
#include "elog.h"
#include "mid_circular_buffer.h"

#include "bsp_uart_driver.h"

#define FRAME_NOT_DETECT_HEAD (0x01) // status frame head not detected
#define FRAME_HEAD (0x02)            // status frame head detected
#define FRAME_TAIL (0x03)            // status frame tail detected

#define FRAME_HEAD_FLAG (0xFE) // frame head pattern
#define FRAME_TAIL_FLAG (0xFF) // frame tail pattern
void uart_rec_A_func(void *argument);

uint8_t buffer1[1] = {0};
uint8_t buffer2[2] = {0};

#endif // end __UART_PARSE_TASK_H__
