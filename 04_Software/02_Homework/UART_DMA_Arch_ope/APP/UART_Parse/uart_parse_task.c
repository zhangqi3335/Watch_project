
#include "uart_parse_task.h"

QueueHandle_t queue_irq_rec_A = NULL;

// extern circular_buffer_t *g_circular_buffer_irq_thread;
static circular_buffer_t *g_circular_buffer_from_driver = NULL;
static uint8_t app_subscribe_number = 0; // number of subscribers to app
// APP订阅者注册数组
app_subscriber_t app_subscribers[PROTO_MAX_SUBSCRIBERS];

uint8_t app_subscribe_array(uint8_t cmd[PROTO_MAX_CMD_SUBSCRIBERS], QueueHandle_t rx_handle)
{
	for (uint8_t i = 0; i < PROTO_MAX_CMD_SUBSCRIBERS; i++)
	{
		if (app_subscribe_number >= PROTO_MAX_SUBSCRIBERS)
		{
			return 0x00;
		}
		app_subscribers[app_subscribe_number].cmd[i] = cmd[i];
		app_subscribe_number++;
	}
	app_subscribers[app_subscribe_number].rx_handle = rx_handle;
	return 0x01;
}

void uart_rec_A_func(void *argument)
{
	/* USER CODE BEGIN uart_rec_A_func */
	/* Infinite loop */
	static uint32_t receive_data = 0;
	static uint32_t receive_data_temp = 0;
	log_i("uart_rec_A_func Init Success");
	queue_irq_rec_A = NULL;
	queue_irq_rec_A = xQueueCreate(1, 4);
	if (NULL == queue_irq_rec_A)
	{
		log_i("queue_irq_rec_A Init failed");
	}
	else
	{
		log_i("queue_irq_rec_A Init Success");
		log_i("queue_irq_rec_A = [%x]", queue_irq_rec_A);
	}
	for (;;)
	{

		xQueueReceive(queue_irq_rec_A, &receive_data, 0xffff);
		log_i("end receive_data from queue_irq_rec_A = [%x]", receive_data);

		g_circular_buffer_from_driver = get_circular_buffer();
		if (NULL == g_circular_buffer_from_driver)
		{
			log_e("error with NULL pointer of g_circular_buffer");
			return;
		}
		while (0x00 != buffer_is_empty(g_circular_buffer_from_driver))
		{
			uint8_t temp_data = 0;
			static uint8_t data_counter = 0;
			static uint8_t temp_data_array[20] = {0x00};
			if (0x00 == get_data(g_circular_buffer_from_driver, &temp_data))
			{
				// log_i("buffer_get_success");
			}
			osDelay(3);
			// log_i("g_circular_buffer_from_driver = [%d]", temp_data);

			// parse the data
			static uint32_t frame_status = FRAME_NOT_DETECT_HEAD;
			switch (frame_status)
			{
			case FRAME_NOT_DETECT_HEAD:
				/* code */
				if (FRAME_HEAD_FLAG == temp_data)
				{
					frame_status = FRAME_HEAD;
					log_i("FRAME_HEAD_DETECT");
				}
				break;
			case FRAME_HEAD:
				if (FRAME_TAIL_FLAG == temp_data)
				{
					log_i("FRAME_TAIL_DETECT");
					uint32_t data_summ_temp = 0;
					uint32_t data_sum = temp_data_array[data_counter - 1];
					log_i("data_sum = [%d]", data_sum);
					// 计算校验和
					for (uint8_t i = 0; i < data_counter - 1; i++)
					{
						data_summ_temp += temp_data_array[i];
					}
					log_i("data_summ_temp = [%d]", data_summ_temp);
					if (data_sum == data_summ_temp)
					{

						for (uint8_t j = 0; j < data_counter - 1; j++)
						{
							log_i("data[%d] = [%d]", j, temp_data_array[j]);
						}
						// To Do:
						// 协议: HEAD(1) + CMD(1) + LEN(1) + DATA(N) + CHK(1) + TAIL(1)
						// 1.解析协议中的CMD数据，CMD在数据头后的第一个字节
						log_i("CMD = [%d]", temp_data_array[0]);
						// 2.解析协议中的数据，数据在Len字节后面的数据
						app_msg_t temp_msg = {.data_cnt = receive_data_temp};
						for (uint32_t k = 0; k < temp_data_array[1]; k++)
						{
							temp_msg.data[k] = temp_data_array[k + 1];
						}
						// 3.将解析后的数据发送给APP订阅者
						for (uint32_t i = 0; i < PROTO_MAX_SUBSCRIBERS; i++)
						{
							for (uint32_t j = 0; j < PROTO_MAX_CMD_SUBSCRIBERS; j++)
							{
								if (app_subscribers[i].cmd[j] == temp_data_array[0])
								{
									xQueueSend(app_subscribers[i].rx_handle, &temp_msg, 0);
								}
							}
						}

						data_counter = 0;
						memset(temp_data_array, 0, 20);
						log_i("data_sum_check_success");
					}
					else
					{
						log_e("data_sum_check_failed");
						data_counter = 0;
						memset(temp_data_array, 0, 20);
					}
					frame_status = FRAME_NOT_DETECT_HEAD;
				}
				else
				{
					log_i(" FRAME_DETECT: [%d]", temp_data);
					temp_data_array[data_counter] = temp_data;
					data_counter++;
					log_i("data_counter = [%d]", data_counter);
				}
				break;

				// default:
				// 	break;
			}
		}
	}
	/* USER CODE END uart_rec_A_func */
}
