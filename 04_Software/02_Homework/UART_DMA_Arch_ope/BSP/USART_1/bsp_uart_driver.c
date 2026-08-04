#include "bsp_uart_driver.h"

#define BUFFER_A 0
#define BUFFER_B 1
#define ISR_SEND_TO_TASK 0xA1A2A3A4
#define FRONT_SEND_TO_END 0xA7A8A9A1
extern QueueHandle_t queue_irq_rec_A;

#if 0  // AB-Buffer
uint8_t flag_AB = BUFFER_A;
uint8_t g_data_buffer_A[1] = {0x00};
uint8_t g_data_buffer_B[1] = {0x00};
#endif // end of AB-Buffer

#if 1 // circular buffer

uint8_t g_data_buffer = 0;

#endif                                                         // end of circular buffer
static circular_buffer_t *g_circular_buffer_irq_thread = NULL; //.bss
static QueueHandle_t queue_uart_irq_thread = NULL;

void uart_driver_func(void *argument)
{
    /* USER CODE BEGIN uart_rec_A_func */
    /* DEBUG USART */
    static uint32_t receive_data = 0;
    // 0. 创建环形缓冲区
    circular_buffer_t *p_circular_buffer = create_empty_circular_buffer();
    if (NULL == p_circular_buffer)
    {
        log_e("circular_buffer create failed");
    }
    // 0.1 mount the global pointer to circular_buffer
    g_circular_buffer_irq_thread = p_circular_buffer;
    log_i("circular_buffer create Success.");

    // create the queue for uart irq thread to send data to parse thread
    queue_uart_irq_thread = NULL;
    queue_uart_irq_thread = xQueueCreate(1, 4);
    if (NULL == queue_uart_irq_thread)
    {
        log_i("queue_uart_irq_thread Init failed");
    }
    else
    {
        log_i("queue_uart_irq_thread Init Success");
        log_i("queue_uart_irq_thread = [%x]", queue_uart_irq_thread);
    }

    // start the uart receive interrupt
    HAL_StatusTypeDef ret = HAL_OK;
    // signal the uart receive interrupt to start receiving data
    // ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
    // 串口空闲中断和半满全满
    ret = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_circular_buffer_irq_thread->data, CIRCULAR_BUFFER_SIZE); // CIRCULAR_BUFFER_SIZE);
    if (HAL_OK == ret)
    {
        log_i("HAL UART Init Success.");
    }
    else
    {
        log_i("HAL UART Init Failed");
    }

    for (;;)
    {
        // 判断是否中断发送给前端
        xQueueReceive(queue_uart_irq_thread, &receive_data, 0xffff);
        log_i("front receive_data from queue_uart_irq_thread = [%x]", receive_data);
        if (ISR_SEND_TO_TASK == receive_data)
        {

            uint32_t send_data_to_end = FRONT_SEND_TO_END;
            // 发送消息给后端
            // BaseType_t ret = xQueueSend(queue_irq_rec_A, &send_data_to_end, 0);
            // 采用二值信号量，== 封装的数量为1的队列
            BaseType_t ret = xQueueGenericSend(queue_irq_rec_A, &send_data_to_end, 0, queueOVERWRITE);
            if (pdTRUE != ret)
            {
                log_e("xQueueSend error");
            }
            else
            {
                log_d("front xQueueSend to end success");
            }

            // uint8_t temp_data = 0;
            // if (0x00 == get_data(g_circular_buffer_irq_thread,
            //                      &temp_data))
            // {
            //     log_i("circular_buffer_get_success");
            // }
            // log_i("buffer_read_out = [%d]", temp_data);
        }
        osDelay(1);
    }
    /* USER CODE END uart_rec_A_func */
}

circular_buffer_t *get_circular_buffer(void)
{
    if (NULL == g_circular_buffer_irq_thread)
    {
        log_e("error with NULL pointer of g_circular_buffer");
        return NULL;
    }
    return g_circular_buffer_irq_thread;
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
    /* NOTE: This function should not be modified, when the callback is needed,
             the HAL_UART_RxCpltCallback could be implemented in the user file
     */
#if 0  // AB buffer test
	log_d("HAL_UART_RxCpltCallback");
	
    
    
    HAL_StatusTypeDef ret = HAL_OK;
    
    if ( BUFFER_A == flag_AB )
    {
        log_d("g_data_buffer_A = [%c]",g_data_buffer_A[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_B, 1);
        if ( HAL_OK != ret )
        {
            log_e("HAL_UART_Receive_IT error!");
        }
        flag_AB = BUFFER_B;
    } 
    else 
    {
        log_d("g_data_buffer_B = [%c]",g_data_buffer_B[0]);
        ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_A, 1);
        if ( HAL_OK != ret )
        {
            log_e("HAL_UART_Receive_IT error!");
        }
        flag_AB = BUFFER_A;
    }
#endif // end of AB buffer test

/*
2 将 1 实现的 ABCDEFG buffer 形式的串口通信架构实现为环形缓冲区
（视频中线程 A 此时强行搬运数据，暂时不需要解析）
1.ABbuffer切换为环形缓冲区
{
  1.1把数据的写入（在单字节中断中）指向环形缓冲区
  测试：写入后，立马读出，验证
}
*/
#if 0 // circular_buffer test
      if(NULL == g_circular_buffer_irq_thread)
      {
          log_e("error with NULL pointer of g_circular_buffer");
          return;
      }
      
      uint8_t ret = 0;
      
      ret = insert_data  (g_circular_buffer_irq_thread,\
                                          g_data_buffer);
      
      if( 0x00 == ret )
    {
        uint8_t temp_data = 0;
        if ( 0x00 == get_data(g_circular_buffer_irq_thread,
                                                &temp_data))
        {
            log_i("circular_buffer_get_success");
        }
        log_i("buffer_read_out = [%d]",temp_data );
    }
      
    HAL_StatusTypeDef ret_1 = HAL_OK;
	
    ret_1 = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
        
        
    if(HAL_OK == ret_1)
    {
        log_i("HAL UART Init Success.");
    } 
    else
    {
        log_i("HAL UART Init Failed");
    }
#endif

#if 1 // parse_thread test
    // 1.把数据存入缓冲区
    if (NULL == g_circular_buffer_irq_thread)
    {
        log_e("error with NULL pointer of g_circular_buffer");
        return;
    }
    BaseType_t ret = pdTRUE;
    insert_data(g_circular_buffer_irq_thread, g_data_buffer);
    // 2.通知任务
    uint32_t send_data_to_task = ISR_SEND_TO_TASK;
    ret = xQueueGenericSendFromISR(queue_uart_irq_thread, &send_data_to_task, 0, queueOVERWRITE);
    if (pdTRUE != ret)
    {
        log_e("xQueueSendFromISR error");
    }
    else
    {
        // log_d("xQueueSendFromISR success");
    }
    // 3.重新使能中断
    HAL_StatusTypeDef ret_1 = HAL_OK;
    ret_1 = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
#endif
    // g_data_buffer_A
    // ret = xQueueSendFromISR( queue_irq_rec_A, &send_data_to_rec_A,NULL);
    // log_d("ret = [%d]",ret);
}
/* USER CODE END 1 */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    log_d("HAL_UARTEx_RxEventCallback = [%d]", Size);
}

void dma_half_irq_callback(uint32_t number_of_data)
{
    log_d("dma_half_irq_callback = [%d]", number_of_data);
    uint32_t head_pos = 0;
    uint32_t current_data_pos = 0;
    uint32_t pos_in_buffer = 0;
    uint32_t add_num = 0;
    if (0x00 == get_head_pos(g_circular_buffer_irq_thread, &head_pos))
    {
        // log_d("head_pos = [%d]", head_pos);
    }
    else
    {
        log_e("get_head_pos error");
    }
    // 总长度 = number_of_data * 2 = CIRCULAR_BUFFER_SIZE；
    // 因为从0开始数，数据到达的位置就是 总长度数值/2 - 1
    current_data_pos = CIRCULAR_BUFFER_SIZE / 2;
    // 对head的当前位置与总长度的一半取余
    pos_in_buffer = head_pos % (CIRCULAR_BUFFER_SIZE / 2);
    // 计算需要增加的数量
    if (pos_in_buffer > current_data_pos)
    {
        add_num = (CIRCULAR_BUFFER_SIZE - pos_in_buffer) + current_data_pos;
    }
    else
    {
        add_num = current_data_pos - pos_in_buffer;
    }
    // log_i("current_data_pos = [%d], pos_in_buffer = [%d], add_num = [%d]", current_data_pos, pos_in_buffer, add_num);
    if (0x00 == head_pos_increment(g_circular_buffer_irq_thread, add_num))
    {
        // log_i("head_pos_increment success,head_pos = [%d]", g_circular_buffer_irq_thread->head);
    }
    else
    {
        log_e("head_pos_increment error");
    }
    // 发送消息给前端
    uint32_t send_data_to_task = ISR_SEND_TO_TASK;
    BaseType_t ret = pdTRUE;
    ret = xQueueGenericSendFromISR(queue_uart_irq_thread, &send_data_to_task, 0, queueOVERWRITE);
    if (pdTRUE != ret)
    {
        log_e("xQueueSendFromISR error");
    }
    else
    {
        log_d("dma_half_irq_callback xQueueSendFromISR success");
    }
}
void dma_cplt_irq_callback(uint32_t number_of_data)
{
    log_d("dma_cplt_irq_callback = [%d]", number_of_data);
    uint32_t head_pos = 0;
    uint32_t current_data_pos = 0;
    uint32_t pos_in_buffer = 0;
    uint32_t add_num = 0;
    if (0x00 == get_head_pos(g_circular_buffer_irq_thread, &head_pos))
    {
        // log_d("head_pos = [%d]", head_pos);
    }
    else
    {
        log_e("get_head_pos error");
    }
    // 总长度 = number_of_data * 2 = CIRCULAR_BUFFER_SIZE；
    // 因为从0开始数，数据到达的位置就是 总长度数值/2 - 1
    current_data_pos = CIRCULAR_BUFFER_SIZE;
    // 对head的当前位置与总长度的一半取余
    pos_in_buffer = head_pos % CIRCULAR_BUFFER_SIZE;
    // 计算需要增加的数量
    if (pos_in_buffer > current_data_pos)
    {
        add_num = (CIRCULAR_BUFFER_SIZE - pos_in_buffer) + current_data_pos;
    }
    else
    {
        add_num = current_data_pos - pos_in_buffer;
    }
    add_num = current_data_pos - pos_in_buffer;
    // log_i("current_data_pos = [%d], pos_in_buffer = [%d], add_num = [%d]", current_data_pos, pos_in_buffer, add_num);
    if (0x00 == head_pos_increment(g_circular_buffer_irq_thread, add_num))
    {
        // log_i("head_pos_increment success,head_pos = [%d]", g_circular_buffer_irq_thread->head);
    }
    else
    {
        log_e("head_pos_increment error");
    }
    // 发送消息给前端
    uint32_t send_data_to_task = ISR_SEND_TO_TASK;
    BaseType_t ret = pdTRUE;
    ret = xQueueGenericSendFromISR(queue_uart_irq_thread, &send_data_to_task, 0, queueOVERWRITE);
    if (pdTRUE != ret)
    {
        log_e("xQueueSendFromISR error");
    }
    else
    {
        log_d("dma_cplt_irq_callback xQueueSendFromISR success");
    }
}
void uart_idle_irq_callback(uint32_t number_of_data)
{
    log_d("uart_idle_irq_callback = [%d]", number_of_data);
    uint32_t head_pos = 0;
    uint32_t data_pos = 0;
    uint32_t pos_in_buffer = 0;
    uint32_t add_num = 0;
    if (0x00 == get_head_pos(g_circular_buffer_irq_thread, &head_pos))
    {
        // log_d("head_pos = [%d]", head_pos);
    }
    else
    {
        log_e("get_head_pos error");
    }
    data_pos = number_of_data;
    // 对head的当前位置与总长度的一半取余
    pos_in_buffer = head_pos % CIRCULAR_BUFFER_SIZE;
    // 计算需要增加的数量,可能为负数
    if (pos_in_buffer > data_pos)
    {
        add_num = (CIRCULAR_BUFFER_SIZE - pos_in_buffer) + data_pos;
    }
    else
    {
        add_num = data_pos - pos_in_buffer;
    }
    // log_i("data_pos = [%d], pos_in_buffer = [%d], add_num = [%d]", data_pos, pos_in_buffer, add_num);
    if (0x00 == head_pos_increment(g_circular_buffer_irq_thread, add_num))
    {
        // log_i("head_pos_increment success,head_pos = [%d]", g_circular_buffer_irq_thread->head);
    }
    else
    {
        log_e("head_pos_increment error");
    }
    // 发送消息给前端
    uint32_t send_data_to_task = ISR_SEND_TO_TASK;
    BaseType_t ret = pdTRUE;
    ret = xQueueGenericSendFromISR(queue_uart_irq_thread, &send_data_to_task, 0, queueOVERWRITE);
    if (pdTRUE != ret)
    {
        log_e("xQueueSendFromISR error");
    }
    else
    {
        log_d("uart_idle_irq_callback xQueueSendFromISR success");
    }
}