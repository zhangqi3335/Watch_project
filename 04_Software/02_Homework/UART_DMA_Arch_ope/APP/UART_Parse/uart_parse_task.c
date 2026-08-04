
#include "uart_parse_task.h"

QueueHandle_t queue_irq_rec_A = NULL;

// extern circular_buffer_t *g_circular_buffer_irq_thread;
static circular_buffer_t *g_circular_buffer_from_driver = NULL;
void uart_rec_A_func(void *argument)
{
	/* USER CODE BEGIN uart_rec_A_func */
	/* Infinite loop */
	static uint32_t receive_data = 0;
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
