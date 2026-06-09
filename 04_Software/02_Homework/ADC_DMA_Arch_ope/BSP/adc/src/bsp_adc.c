/******************************************************************************
 * Copyright (C)
 *
 * All Rights Reserved.
 *
 * @file bsp_adc.c
 *
 * @author 张奇
 *
 * @brief Board Support Package for ADC control.
 *
 * @version V1.0 <2026-06-04>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "cmsis_os.h"
#include "main.h"
#include "elog.h"
#include "adc.h"
#include "bsp_adc.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define ADC_BUFFER_SIZE 32
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
osThreadId_t ADCTask1Handle;
const osThreadAttr_t ADCTask1_attributes = {
    .name = "ADCTask1",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityAboveNormal,
};

osThreadId_t ADCTask2Handle;
const osThreadAttr_t ADCTask2_attributes = {
    .name = "ADCTask2",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
// 存储ADC的数据
uint32_t *gp_buffer1 = NULL;
uint32_t *gp_buffer2 = NULL;
// 传输的队列
QueueHandle_t xQueue = NULL;
SemaphoreHandle_t xMutex = NULL;
// 指针状态标志位
volatile uint8_t g_usingbuf = 1;
/* Private function prototypes ----------------------------------------------*/
/* Create the thread(s) */
void StartADCTask1(void *argument);
void StartADCTask2(void *argument);
void BSP_ADC_Task_Init(void);
void BSP_ADC_variable_init(void);
/* Public functions ---------------------------------------------------------*/
/**
 * @brief  Function implementing the ADCTask1 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartADCTask1*/
void StartADCTask1(void *argument)
{
    /* USER CODE BEGIN StartADCTask1*/
    /* Infinite loop */
    for (;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // 记录刚完成的是哪个Buffer，准备切DMA
        uint32_t *p_complete = NULL;
        uint32_t *p_next = NULL;
        if (g_usingbuf == 1)
        {
            p_complete = gp_buffer1;
            p_next = gp_buffer2;
        }
        else
        {
            p_complete = gp_buffer2;
            p_next = gp_buffer1;
        }
        xSemaphoreTake(xMutex, portMAX_DELAY);
        if (HAL_ADC_Start_DMA(&hadc1, p_next, ADC_BUFFER_SIZE) != HAL_OK)
        {
            // Handle ADC DMA start failure
            log_e("Failed to start ADC DMA");
        }
        g_usingbuf = (g_usingbuf == 1) ? 2 : 1; // 切换标志位
        xSemaphoreGive(xMutex);
        xQueueSend(xQueue, &p_complete, portMAX_DELAY); // 将完成的缓冲区指针发送到队列
    }
    /* USER CODE END StartADCTask1*/
    osDelay(10);
}

/**
 * @brief  Function implementing the ADCTask2 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartADCTask2*/
void StartADCTask2(void *argument)
{
    /* USER CODE BEGIN StartADCTask2*/
    /* Infinite loop */
    for (;;)
    {
        uint32_t *p_buffer = NULL;
        xQueueReceive(xQueue, &p_buffer, portMAX_DELAY); // 从队列中取出完成的缓冲区指针
        xSemaphoreTake(xMutex, portMAX_DELAY);
        // Process the completed buffer
        for (int i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            float adc_voltage = (float)((p_buffer[i]) * 3.3 / 4096); // 读取ADC值
            log_i("ADC Voltage: %fV", adc_voltage);
        }
        xSemaphoreGive(xMutex);
    }
    /* USER CODE END StartADCTask2*/
}

void BSP_ADC_Init(void)
{
    BSP_ADC_variable_init();
    BSP_ADC_Task_Init();
}

void BSP_ADC_Start(void)
{
    if (HAL_ADC_Start_DMA(&hadc1, gp_buffer1, ADC_BUFFER_SIZE) != HAL_OK)
    {
        // Handle ADC DMA start failure
        log_e("Failed to start ADC DMA");
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(ADCTask1Handle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/* Private functions --------------------------------------------------------*/
void BSP_ADC_Task_Init(void)
{
    ADCTask1Handle = osThreadNew(StartADCTask1, NULL, &ADCTask1_attributes);
    ADCTask2Handle = osThreadNew(StartADCTask2, NULL, &ADCTask2_attributes);
}

void BSP_ADC_variable_init(void)
{
    gp_buffer1 = (uint32_t *)malloc(ADC_BUFFER_SIZE * sizeof(uint32_t));
    gp_buffer2 = (uint32_t *)malloc(ADC_BUFFER_SIZE * sizeof(uint32_t));
    if (gp_buffer1 == NULL || gp_buffer2 == NULL)
    {
        // Handle memory allocation failure
        log_e("Failed to allocate memory for ADC buffers");
        while (1)
        {
        };
    }
    memset(gp_buffer1, 0, ADC_BUFFER_SIZE * sizeof(uint32_t));
    memset(gp_buffer2, 0, ADC_BUFFER_SIZE * sizeof(uint32_t));
    xQueue = xQueueCreate(1, sizeof(uint32_t *));
    if (xQueue == NULL)
    {
        // Handle queue creation failure
        log_e("Failed to create queue");
        while (1)
        {
        };
    }
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL)
    {
        // Handle mutex creation failure
        log_e("Failed to create mutex");
        while (1)
        {
        };
    }
}
