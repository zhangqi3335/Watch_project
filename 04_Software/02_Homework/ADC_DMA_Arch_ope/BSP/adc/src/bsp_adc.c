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
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityAboveNormal,
};

osThreadId_t ADCTask2Handle;
const osThreadAttr_t ADCTask2_attributes = {
    .name = "ADCTask2",
    .stack_size = 512 * 4,
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
volatile uint32_t g_isr_count = 0;
volatile uint32_t task1_alive = 0;
volatile uint32_t task2_alive = 0;
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
    BSP_ADC_Start();
    /* Infinite loop */
    for (;;)
    {
        task1_alive++;
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
        HAL_ADC_Stop_DMA(&hadc1);
        if (HAL_ADC_Start_DMA(&hadc1, p_next, ADC_BUFFER_SIZE) != HAL_OK)
        {
            // Handle ADC DMA start failure
            elog_e("DEBUG", "Failed to start ADC DMA");
        }
        g_usingbuf = (g_usingbuf == 1) ? 2 : 1; // 切换标志位
        xSemaphoreGive(xMutex);
        xQueueSend(xQueue, &p_complete, portMAX_DELAY); // 将完成的缓冲区指针发送到队列
        osDelay(10);
    }
    /* USER CODE END StartADCTask1*/
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
        task2_alive++;
        uint32_t *p_buffer = NULL;
        elog_i("DEBUG", "ADCTask2 received buffer first");
        xQueueReceive(xQueue, &p_buffer, portMAX_DELAY); // 从队列中取出完成的缓冲区指针
        elog_i("DEBUG", "ADCTask2 received buffer %p", p_buffer);
        xSemaphoreTake(xMutex, portMAX_DELAY);
        // Process the completed buffer
        elog_i("DEBUG", "Processing ADC data:");
        float voltage = (float)p_buffer[0] * 3.3f / 4096; // 12位ADC，参考电压3.3V
        elog_i("DEBUG", "ADC Voltage: %fV", voltage);
        float voltage2 = (float)p_buffer[31] * 3.3f / 4096; // 12位ADC，参考电压3.3V
        elog_i("DEBUG", "ADC Voltage: %fV", voltage2);
        xSemaphoreGive(xMutex);
        osDelay(100);
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
    int ret = HAL_ADC_Start_DMA(&hadc1, gp_buffer1, ADC_BUFFER_SIZE);
    if (ret != HAL_OK)
    {
        // Handle ADC DMA start failure
        elog_e("ADC", "Failed to start ADC DMA");
    }
    //elog_i("ADC", "ADC DMA started successfully:ret=%d", ret);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    g_isr_count++;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(ADCTask1Handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
} /* Private functions --------------------------------------------------------*/
void BSP_ADC_Task_Init(void)
{
    ADCTask1Handle = osThreadNew(StartADCTask1, NULL, &ADCTask1_attributes);
    ADCTask2Handle = osThreadNew(StartADCTask2, NULL, &ADCTask2_attributes);
    configASSERT(ADCTask1Handle);
    configASSERT(ADCTask2Handle);
}

void BSP_ADC_variable_init(void)
{
    gp_buffer1 = (uint32_t *)malloc(ADC_BUFFER_SIZE * sizeof(uint32_t));
    gp_buffer2 = (uint32_t *)malloc(ADC_BUFFER_SIZE * sizeof(uint32_t));
    if (gp_buffer1 == NULL || gp_buffer2 == NULL)
    {
        // Handle memory allocation failure
        elog_e("DEBUG", "Failed to allocate memory for ADC buffers");
        while (1)
        {
        };
    }
    memset(gp_buffer1, 0, ADC_BUFFER_SIZE * sizeof(uint32_t));
    memset(gp_buffer2, 0, ADC_BUFFER_SIZE * sizeof(uint32_t));
    xQueue = xQueueCreate(4, sizeof(uint32_t *));
    if (xQueue == NULL)
    {
        // Handle queue creation failure
        elog_e("DEBUG", "Failed to create queue");
        while (1)
        {
        };
    }
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL)
    {
        // Handle mutex creation failure
        elog_e("DEBUG", "Failed to create mutex");
        while (1)
        {
        };
    }
}
