/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include <SEGGER_RTT.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>

// ================== 切换输出通道 ==================
#define LOG_OUTPUT_RTT      // ← 有 J-Link 用这行
// #define LOG_OUTPUT_UART   // ← 没 J-Link 改成这行
// =================================================
/*--------------Prviate variables --------------*/
static SemaphoreHandle_t log_mutex;

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    log_mutex = xSemaphoreCreateMutex();
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

    /* add your code here */

}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
#if defined(LOG_OUTPUT_RTT)
    #include "SEGGER_RTT.h"
    void elog_port_output(const char *log, size_t size) {
        SEGGER_RTT_Write(0, log, size);
    }

#elif defined(LOG_OUTPUT_UART)
    #include "usart.h"
    void elog_port_output(const char *log, size_t size) {
        HAL_UART_Transmit(&huart1, (uint8_t *)log, size, 100);
    }
#else
    void elog_port_output(const char *log, size_t size) {
        // 空实现，不输出
    }
#endif

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    xSemaphoreTake(log_mutex, portMAX_DELAY);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    xSemaphoreGive(log_mutex);
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    /* add your code here */
    static char tim_buf[16];
    sprintf(tim_buf, "%lu", xTaskGetTickCount());
    return tim_buf;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "ADC+DMA";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    return pcTaskGetName(NULL);
}
