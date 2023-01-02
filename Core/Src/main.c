/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stlogo.h"
#include "pa3SDRAM.h"
#include "cmsis_os.h"
#include "pa3Timers.h"
#include "uart.h"
#include "dma.h"
#include "can.h"
#include "obd2.h"
#include "retarget.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define __MEASURE__
#ifdef __MEASURE__
#define __DELAY 2
#else
#define __DELAY 100
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
__IO uint8_t DemoIndex = 0;
__IO uint8_t NbLoop = 1;
__IO uint32_t SdmmcTest = 0;
__IO uint32_t SdramTest = 0;


osThreadId_t Task1Handle;
const osThreadAttr_t Task1_attributes = {
  .name = "Task 1",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task 2",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osMessageQueueId_t mid_MsgQueue;                // message queue id


TIM_HandleTypeDef    	TIM3Handle;
uint16_t timer_val_start, timer_val_end;
uint16_t elapsed_rows, elapsed_cols, elapsed_dma;


UART_HandleTypeDef 		UART1Handle;
DMA_HandleTypeDef    	DMA2_SDRAM_Handle;
extern SDRAM_HandleTypeDef 			sdramHand;

int count = 0;
osMutexId_t  mutex;

extern ALIGN_32BYTES (uint32_t sdram_read_Buffer[SDRAM_BUFFER_SIZE]);



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
void StartDefaultTask(void *argument);
static void MPU_Config(void);
void SystemClock_Config(void);
//static void Display_DemoDescription(void);
static void CPU_CACHE_Enable(void);

/* USER CODE BEGIN PFP */
void Task1(void *argument);
void Task2(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t  lcd_status = LCD_OK;
  char time_str1[40];
  char time_str2[20];

  /* Configure the MPU attributes */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_RED);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

    /* Init timers */
#ifdef __USE_TIM3_BASE__
  Init_TIM3(&TIM3Handle);
#endif

  /* Configure the Tamper push-button in GPIO Mode */
  BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);

  //if (CAN_Config_LoopBack() != HAL_OK) {
  if (CAN1_Config(CAN_LOOPBACK_MODE) != HAL_OK) {
	  Error_Handler();
  }
  //CAN_SendReceiveMessage();
  //OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
  //OBD2_ReceiveData();



  /* Init UART1*/
  if (USART1_Init(&UART1Handle) != HAL_OK){
	  Error_Handler();
  }
  RetargetInit(&UART1Handle);

  // Configure DMA2 for UART1:
  if (DMA2_UART1_Config() != HAL_OK) {
	  Error_Handler();
  }


  // Configure DMA2 for SDRAM:
  if (DMA2_SDRAM_Config(&DMA2_SDRAM_Handle) != HAL_OK) {
	  Error_Handler();
  }
  // Link DMA2_SDRAM_Handle to DMA handle field in SDRAM handle
  __HAL_LINKDMA(&sdramHand, hdma, DMA2_SDRAM_Handle);


  /* Initialize the LCD */
  lcd_status = BSP_LCD_Init();
  while(lcd_status != LCD_OK);

  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);


  BSP_LCD_ReadPixel(BSP_LCD_GetXSize()-1, BSP_LCD_GetYSize()-1);

  BSP_LCD_Clear(LCD_COLOR_WHITE);
  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(0);
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  /* Clear the LCD */
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  /* Set the LCD Text Color */
  //BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 10+20, (uint8_t *)"STM32F769I BSP", CENTER_MODE);
#ifdef __VGRS__
  BSP_LCD_DisplayStringAt(0, 35+20, (uint8_t *)"Vgrajeni sistemi 2022", CENTER_MODE);
#endif
#ifdef __ORS__
  BSP_LCD_DisplayStringAt(0, 35+20, (uint8_t *)"Organizacija racunalniskih sistemov 2022", CENTER_MODE);
#endif


#ifdef __ORS__

  Init_SDRAM();
  simple_SDRAM_test();

  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  BSP_LCD_DisplayStringAt(30, 95+80, (uint8_t *)"Vsebina v SDRAM:", LEFT_MODE);
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_DisplayStringAt(60, 120+80, (uint8_t *)sdram_read_Buffer, LEFT_MODE);

  //HAL_UART_Transmit(&UART1Handle, (uint8_t *)"STM32F769I \n", strlen("STM32F769I \n"), HAL_MAX_DELAY);
  //HAL_UART_Transmit(&UART1Handle, (uint8_t *)"Vsebina v SDRAM: ", strlen("Vsebina v SDRAM: "), HAL_MAX_DELAY);
  //HAL_UART_Transmit(&UART1Handle, (uint8_t *)sdram_read_Buffer, strlen(sdram_read_Buffer), HAL_MAX_DELAY);
  printf("STM32F769I BSP \n");
  printf("Organizacija racunalniskih sistemov 2022 \n");
  printf("Vsebina v SDRAM:  \n");
  printf("%s", sdram_read_Buffer);



  HAL_Delay(500);
  SDRAM_init_matrices();
  SDRAM_write_matrix();



  while (1){



	  // Row-major order access:
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_mat_row_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_rows = timer_val_end - timer_val_start;
	  else
		  elapsed_rows = timer_val_end + (65536-timer_val_start);

	  if (SDRAM_compare_matrices()==0) {
		  BSP_LED_On(LED_GREEN);
	  	  HAL_Delay(__DELAY);
	  }
	  else Error_Handler();
	  BSP_LED_Off(LED_GREEN);

	  // column-major order access:
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_mat_col_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_cols = timer_val_end - timer_val_start;
	  else
		  elapsed_cols = timer_val_end + (65536-timer_val_start);


	  if (SDRAM_compare_matrices()==0) {
		  BSP_LED_On(LED_RED);
	  	  HAL_Delay(__DELAY);
	  }
	  else Error_Handler();
	  BSP_LED_Off(LED_RED);


	  // row-major access using DMA:
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	  timer_val_start = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  SDRAM_DMA_mat_row_access_test();
	  timer_val_end = __HAL_TIM_GET_COUNTER(&TIM3Handle);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	  if (timer_val_end > timer_val_start)
		  elapsed_dma = timer_val_end - timer_val_start;
	  else
		  elapsed_dma = timer_val_end + (65536-timer_val_start);

	  if (SDRAM_compare_matrices()==0) {
		  BSP_LED_On(LED_RED);
		  BSP_LED_On(LED_GREEN);
	  	  HAL_Delay(__DELAY);
	  }
	  else Error_Handler();
	  BSP_LED_Off(LED_RED);
	  BSP_LED_Off(LED_GREEN);


	  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	  BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/2 + 35, BSP_LCD_GetXSize(), 80);
	  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);

	  sprintf(time_str1, "Rows:%5dus  ", elapsed_rows);
	  sprintf(time_str2, "Cols:%5dus  ", elapsed_cols);
	  strcat(time_str1, time_str2);
	  sprintf(time_str2, "DMA:%5dus", elapsed_dma);
	  strcat(time_str1, time_str2);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 50, (uint8_t *)"FMC SDRAM TEST", CENTER_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 80, (uint8_t *)time_str1, CENTER_MODE);



	  // Raje uporabi DMA prenos kot pa printf, ki dela na polling
	  //printf("%s \n", time_str1);
	  strcat(time_str1, "  \n");
	  HAL_UART_Transmit_DMA(&UART1Handle, (uint8_t *)time_str1, strlen(time_str1));

	  HAL_Delay(__DELAY);

	  SDRAM_write_const();

  }


#endif

  //BSP_LCD_SetFont(&Font16);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  //BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/2 + 35, BSP_LCD_GetXSize(), 60);
  BSP_LCD_FillRect(0, 100, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(0, 220+4, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(0, 340+8, BSP_LCD_GetXSize()/3-2, 120);

  BSP_LCD_FillRect(BSP_LCD_GetXSize()/3+2, 100, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(BSP_LCD_GetXSize()/3+2, 220+4, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(BSP_LCD_GetXSize()/3+2, 340+8, BSP_LCD_GetXSize()/3-2, 120);

  BSP_LCD_FillRect(2*BSP_LCD_GetXSize()/3+2, 100, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(2*BSP_LCD_GetXSize()/3+2, 220+4, BSP_LCD_GetXSize()/3-2, 120);
  BSP_LCD_FillRect(2*BSP_LCD_GetXSize()/3+2, 340+8, BSP_LCD_GetXSize()/3-2, 120);

  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);

  BSP_LCD_DisplayStringAt(90, 105, (uint8_t *)"RPM", LEFT_MODE);
  BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/3+2+90, 105, (uint8_t *)"KM/H", LEFT_MODE);
  BSP_LCD_DisplayStringAt(2*BSP_LCD_GetXSize()/3+2+90, 105, (uint8_t *)"LOAD", LEFT_MODE);

  BSP_LCD_DisplayStringAt(35, 105 + 120 + 4, (uint8_t *)"COOLANT TEMP", LEFT_MODE);
  BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/3+2+60, 105 + 120+4, (uint8_t *)"OIL TEMP", LEFT_MODE);
  BSP_LCD_DisplayStringAt(2*BSP_LCD_GetXSize()/3+2+60, 105 + 120+4, (uint8_t *)"AIR TEMP", LEFT_MODE);
  //BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 30, (uint8_t *)"Press User button to start :", CENTER_MODE);


  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  mutex = osMutexNew(NULL);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  mid_MsgQueue = osMessageQueueNew	(16, sizeof(CAN_OBD2_MSGQUEUE_OBJ_t), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Task1Handle = osThreadNew(Task1, NULL, &Task1_attributes);
  Task2Handle = osThreadNew(Task2, NULL, &Task2_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}






/* USER CODE BEGIN 4 */

/**
  * @brief System Clock Configuration
  * @retval None
  */

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // HSE = 25 MHHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;	// PLL HSE DIV (1Mhz)
  RCC_OscInitStruct.PLL.PLLN = 400; // PLL MUL (400 MHz)
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // (200 MhZ)
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // SysCLK = 200 MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;	// AHB CLK = 200 MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;		// APB1 CLK = 50 MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;		// APB2 CLK = 100 MHz

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}



/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU QSPI flash */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;


  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes FMC control registers */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xA0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}


/**
  * @brief  Check for user input
  * @param  None
* @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_WAKEUP) == GPIO_PIN_SET)
  {
    while (BSP_PB_GetState(BUTTON_WAKEUP) == GPIO_PIN_SET);
    return 1 ;
  }
  return 0;
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BSP_LED_Toggle(LED_GREEN);
}



void Task1(void *argument)
{
  /* USER CODE BEGIN 5 */
  CAN_OBD2_MSGQUEUE_OBJ_t queue_data;
  osStatus_t status = osOK;
  uint32_t size;
  uint8_t pid;

  float fvalue;
  uint32_t ui32value;
  uint8_t* obd_response[8];



  /* Infinite loop */
  for(;;)
  {
	osDelay(100);
    //size = osMessageQueueGetCount(mid_MsgQueue);
    //if (size != 0)
	status = osMessageQueueGet(mid_MsgQueue, &queue_data, NULL, osWaitForever);
    if (status == osOK){
    	//pid = queue_data.pid - 0x40;	// ECU responds same as query, except that 40h is added to the service value (PID)
    	pid = queue_data.pid;

    	switch (pid){
    	case OBD2_PID_PIDS_SUPPORTED_01_20:
    		break;
    	case OBD2_PID_FUEL_SYS_STATUS:
    		break;
    	case OBD2_PID_ENGINE_COOLANT_TEMP:
    		ui32value = OBD2DecodeEngineCoolantTemp(queue_data.OBDData);
    		sprintf(obd_response, "%d", ui32value);
    		BSP_LCD_DisplayStringAt(90, 105 + 160 + 4, obd_response, LEFT_MODE);
    		break;
    	case OBD2_PID_FUEL_PRESSURE:
    		break;
    	case OBD2_PID_ENGINE_SPEED:
    		ui32value = OBD2DecodeEngineSpeed(queue_data.OBDData);
    		sprintf(obd_response, "%d", ui32value);
    		BSP_LCD_DisplayStringAt(90, 145, obd_response, LEFT_MODE);
    		break;
    	case OBD2_PID_VEHICLE_SPEED:
    		ui32value = OBD2DecodeVehicleSpeed(queue_data.OBDData);
    		sprintf(obd_response, "%d", ui32value);
    		BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/3+2+90, 145, obd_response, LEFT_MODE);

    		break;
    	case OBD2_PID_ENGINE_LOAD:
    		fvalue = OBD2DecodeEngineLoad(queue_data.OBDData);
    		sprintf(obd_response, "%.1f %", fvalue);
    		BSP_LCD_DisplayStringAt(2*BSP_LCD_GetXSize()/3+2+90, 145, obd_response, LEFT_MODE);

    		break;
    	case OBD2_PID_INTAKE_AIR_TEMP:
    		ui32value = OBD2DecodeIntakeAirTemp(queue_data.OBDData);
    		sprintf(obd_response, "%d", ui32value);
    		BSP_LCD_DisplayStringAt(2*BSP_LCD_GetXSize()/3+2+90, 105 + 160 + 4, obd_response, LEFT_MODE);
    		break;
    	case OBD2_PID_THROTTLE_POSITION:
    		break;
    	case OBD2_PID_ENGINE_OIL_TEMP:
    		ui32value = OBD2DecodeOilTemp(queue_data.OBDData);
    		sprintf(obd_response, "%d", ui32value);
    		BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/3+2+90, 105 + 160 + 4, obd_response, LEFT_MODE);
    		break;
    	case OBD2_PID_ENGINE_FUEL_RATE:
    		break;
    	default:
    		break;
    	}

    }
  }

}


void Task2(void *argument)
{
  /* USER CODE BEGIN 5 */
  CAN_OBD2_MSGQUEUE_OBJ_t queue_element;
  uint32_t size;
  osStatus_t osstatus;

  /* Infinite loop */
  for(;;)
  {
	  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_LOAD);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_COOLANT_TEMP);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_FUEL_PRESSURE);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_SPEED);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_VEHICLE_SPEED);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_INTAKE_AIR_TEMP);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_THROTTLE_POSITION);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_FUEL_TYPE);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_OIL_TEMP);
	  osDelay(50);
	  OBD2_SendQuery(0x01, OBD2_PID_ENGINE_FUEL_RATE);
	  osDelay(50);
  }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  for(;;)
  {
    osDelay(50);
    //osMessageQueueGet(mid_MsgQueue, &queue_element, 0U, 0U);
    //BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 50, (uint8_t *)"Default Task", CENTER_MODE);
    //BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 50, (uint8_t *)(queue_element.buffer), CENTER_MODE);
    osDelay(50);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
