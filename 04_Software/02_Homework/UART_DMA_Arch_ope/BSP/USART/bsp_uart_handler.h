/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_uart_handler.h
 * 
 * @par dependencies 
 * - stdio.h
 * - stdint.h
 * 
 * @author Jack | R&D Dept. | EternalChip 立芯嵌入式
 * 
 * @brief Provide the BSP APIs and corresponding opetions for APP and System.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-09-11
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/
#ifndef __BSP_UART_HANDLER_H__
#define __BSP_UART_HANDLER_H__

#include <stdbool.h>
#include <stdio.h>

#define BSP_HANDLER_HANDLER_DEBUG


// core 传入的接口
typedef struct
{
    /*   1.串口初始化                       */
    int8_t ( *pf_uart_init )             ( void * );
    /*   2.中断回调二次反向注册              */
    int8_t ( *pf_uart_callback_register )( void ( * callback )( void * ));
    /*   3.串口接收缓冲区                   */
    int8_t ( *pf_uart_buffer_register   )( void **);
}core_interface_t;

// OS 传入的接口
typedef struct
{
    /*   1.串口初始化                       */
    int8_t ( *pf_uart_init )             ( void * );
    
}temp_humi_handler_os_interface_t;

/*MCU + OS 输入接口 */
typedef struct
{
    // iic 驱动实例
    iic_driver_interface_t *iic_driver_interface;
    // 时间基准
    timebase_interface_t *timebase_interface;
    // OS 传入的接口
    temp_humi_handler_os_interface_t *os_interface;
		// 
	 yield_interface_t *yield_interface;
}temp_humi_handler_all_input_arg_t;


/**
bsp_temp_humi_xxx_handler_t
 */
typedef struct bsp_temp_humi_xxx_handler
{
    // 底层driver需要的接口
    timebase_interface_t *timebase_interface;
    iic_driver_interface_t *iic_driver_interface;
		yield_interface_t *yield_interface;
    // RTOS传入的接口
    temp_humi_handler_os_interface_t *os_interface;
    // 自身的接口 
 
    // 底层driver实例
    bsp_aht21_driver_t *paht21_instance;
		// event队列句柄
	  void *event_queue_handle;
	  // 是否完成初始化
		bool inited;
    // 获取温度时tick值
    uint32_t last_temp_tick;
    // 获取湿度时tick值
    uint32_t last_humi_tick;
}bsp_temp_humi_xxx_handler_t;





/*handler 对外提供的APP接口 */

/*thread调用函数 */
void temp_humi_handler_thread(void *argument);
/*handler构造函数 */
uint8_t bsp_temp_humi_xxx_handler_inst(
                        bsp_temp_humi_xxx_handler_t *handler_instance,
                        temp_humi_handler_all_input_arg_t *input_arg);


/*对APP提供的数据IO接口 */

/*
@paras
    @param[in]  void *pf_callback : 数据就绪后的通知回调
    @param[in]  (void * const) : 接收到的数据大小
*/
int8_t bsp_uart_subscrib( int8_t (*pf_callback) (void * const ));


#endif /* __BSP_UART_HANDLER_H__ */









