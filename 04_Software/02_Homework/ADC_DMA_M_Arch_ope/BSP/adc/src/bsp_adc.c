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
 * @version V1.0 <2026-07-20>
 *
 *****************************************************************************/
/* Include ------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "elog.h"
#include "SEGGER_RTT.h"
#include "queue.h"
#include "semphr.h"
#include "bsp_adc.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
// static uint32_t ADC_Static_Buffer1[6];
// static uint32_t ADC_Static_Buffer2[6];
uint32_t *pADC_Buffer1 = NULL;
uint32_t *pADC_Buffer2 = NULL;
QueueHandle_t xQueueADC = NULL;
SemaphoreHandle_t xSemaphoreADC = NULL;
SemaphoreHandle_t xMutexADC = NULL;
/* Definitions for ADCTask */
osThreadId_t ADC_ATaskHandle;
const osThreadAttr_t ADC_ATask_attributes = {
    .name = "ADC_ATask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
osThreadId_t ADC_BTaskHandle;
const osThreadAttr_t ADC_BTask_attributes = {
    .name = "ADC_BTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

static uint32_t *g_active_buffer = NULL;   // DMA当前写入
static uint32_t *g_finished_buffer = NULL; // DMA刚完成
/* Private function prototypes ----------------------------------------------*/
static void StartADC_ATask(void *argument);
static void StartADC_BTask(void *argument);
static void BSP_ADC_Variables_Init(void);
/* Public functions ---------------------------------------------------------*/
/**
 * @brief Create ADC tasks for FreeRTOS.
 * @param[in] None
 * @retval None
 */
void Task_ADC_Create(void)
{
    /* Create the thread(s) */
    /* creation of ADC_ATask */
    ADC_ATaskHandle = osThreadNew(StartADC_ATask, NULL, &ADC_ATask_attributes);
    /* creation of ADC_BTask */
    ADC_BTaskHandle = osThreadNew(StartADC_BTask, NULL, &ADC_BTask_attributes);
}

/**
 * @brief Initialize the ADC DMA and buffers.
 * @param[in] None
 * @retval None
 */
void BSP_ADC_Init(void)
{
    BSP_ADC_Variables_Init();
    g_active_buffer = pADC_Buffer1;
    HAL_StatusTypeDef ret = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_active_buffer, 6);
    if (ret != HAL_OK)
    {
        SEGGER_RTT_printf(0, "HAL_ADC_Start_DMA failed! Error Code: %d\r\n", ret);
    }
    else
    {
        SEGGER_RTT_printf(0, "HAL_ADC_Start_DMA started successfully.\r\n");
    }
}

/**
 * @brief rewrite the ADC conversion complete callback function.
 * @param[in] hadc
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        g_finished_buffer = g_active_buffer;
        xSemaphoreGiveFromISR(xSemaphoreADC, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/* Private functions --------------------------------------------------------*/
static void StartADC_ATask(void *argument)
{
    /* USER CODE BEGIN StartADC_ATask */
    uint32_t *gp_ADC_Data = NULL;
    uint32_t *next_buffer;
    /* Infinite loop */
    for (;;)
    {
        log_d("ADC_ATask is running");
        if (xSemaphoreTake(xSemaphoreADC, portMAX_DELAY) == pdTRUE)
        {
            if (xQueuePeek(xQueueADC, &gp_ADC_Data, 0) == pdFALSE)
            {
                // Process the ADC data
                if (xSemaphoreTake(xMutexADC, portMAX_DELAY) == pdTRUE)
                {
                    next_buffer = (g_finished_buffer == pADC_Buffer1) ? pADC_Buffer2 : pADC_Buffer1;
                    HAL_ADC_Stop_DMA(&hadc1);
                    g_active_buffer = next_buffer;
                    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_active_buffer, 6);
                    xQueueSend(xQueueADC, &g_finished_buffer, portMAX_DELAY);
                    xSemaphoreGive(xMutexADC);
                }
                else
                {
                    log_w("ADC mutex is not available, holded by taskB");
                }
            }
        }
        osDelay(100);
    }
    /* USER CODE END StartADC_ATask */
}

static void StartADC_BTask(void *argument)
{
    /* USER CODE BEGIN StartADC_BTask */
    /* Infinite loop */
    for (;;)
    {
        log_d("ADC_BTask is running");
        uint32_t *gp_ADC_Data = NULL;
        if (xQueuePeek(xQueueADC, &gp_ADC_Data, 0) == pdTRUE)
        {
            if (xSemaphoreTake(xMutexADC, portMAX_DELAY) == pdTRUE)
            {
                // Process the ADC datas
                if (xQueueReceive(xQueueADC, &gp_ADC_Data, 0) == pdTRUE)
                {
                    for (uint8_t i = 0; i < 6; i++)
                    {
                        log_i("ADC data[%d]: %lu", i, gp_ADC_Data[i]);
                    }
                    if (gp_ADC_Data == pADC_Buffer1)
                    {
                        log_i("ADC data from buffer 1");
                    }
                    else if (gp_ADC_Data == pADC_Buffer2)
                    {
                        log_i("ADC data from buffer 2");
                    }
                    else
                    {
                        log_w("ADC data is not from buffer 1 or buffer 2");
                    }
                }
                xSemaphoreGive(xMutexADC);
            }
            else
            {
                log_w("ADC mutex is not available, holded by taskA");
            }
        }
        osDelay(100);
    }
    /* USER CODE END StartADC_BTask */
}

/**
 * @brief Initialize the ADC needed variables.
 * @param[in] None
 * @retval None
 */
static void BSP_ADC_Variables_Init(void)
{
    // Initialize the ADC buffers
    pADC_Buffer1 = (uint32_t *)malloc(6 * sizeof(uint32_t));
    pADC_Buffer2 = (uint32_t *)malloc(6 * sizeof(uint32_t));
    // pADC_Buffer1 = ADC_Static_Buffer1;
    // pADC_Buffer2 = ADC_Static_Buffer2;
    if (pADC_Buffer1 == NULL || pADC_Buffer2 == NULL)
    {
        log_e("Failed to allocate memory for ADC buffers");
        return;
    }
    memset(pADC_Buffer1, 0xff, 6 * sizeof(uint32_t));
    memset(pADC_Buffer2, 0xff, 6 * sizeof(uint32_t));
    // initialize the xQueue for ADC data transfer
    xQueueADC = xQueueCreate(2, sizeof(uint32_t *));
    if (xQueueADC == NULL)
    {
        log_e("Failed to create ADC queue");
        return;
    }
    // initialize the xSemaphore and Mutex for ADC data transfer
    xSemaphoreADC = xSemaphoreCreateBinary();
    if (xSemaphoreADC == NULL)
    {
        log_e("Failed to create ADC semaphore");
        return;
    }
    xMutexADC = xSemaphoreCreateMutex();
    if (xMutexADC == NULL)
    {
        log_e("Failed to create ADC mutex");
        return;
    }
}
