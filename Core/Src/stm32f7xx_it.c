/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f7xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
extern SAI_HandleTypeDef haudio_out_sai;
extern DFSDM_Filter_HandleTypeDef       hAudioInTopLeftFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInTopRightFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInButtomLeftFilter;
extern DFSDM_Filter_HandleTypeDef       hAudioInButtomRightFilter;
extern SDRAM_HandleTypeDef 				sdramHandle;
extern SD_HandleTypeDef 				uSdHandle;
extern TIM_HandleTypeDef    			TIM3Handle;
extern UART_HandleTypeDef 				UART1Handle;
extern DMA_HandleTypeDef    			DMA2_SDRAM_Handle;
extern DMA_HandleTypeDef    			DMA2_UART1_Handle;
extern CAN_HandleTypeDef     			CanHandle;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(WAKEUP_BUTTON_PIN);
  HAL_GPIO_EXTI_IRQHandler(TS_INT_PIN);
}


/**
  * @brief  This function handles TIM3 interrupt requests.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void){
  HAL_TIM_IRQHandler(&TIM3Handle);
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UART1Handle);
}

/**
  * @brief  This function handles DMA SDRAM stream interrupt request.
  * @param  None
  * @retval None
  */
void DMA_SDRAM_INSTANCE_IRQHANDLER(void)
{
  /* Check the interrupt and clear flag */
  HAL_DMA_IRQHandler(&DMA2_SDRAM_Handle);
}

/**
  * @brief  This function handles DMA2 Stream 0 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream0_IRQHandler(void)
{
  if(SdmmcTest == 1)
  {
    HAL_DMA_IRQHandler(uSdHandle.hdmarx);
  }
  else if(SdramTest == 1)
  {
    HAL_DMA_IRQHandler(sdramHandle.hdma);
  }
  else
  {
    HAL_DMA_IRQHandler(hAudioInTopLeftFilter.hdmaReg);
  }
}

/**
  * @brief  This function handles DMA2 Stream 5 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream5_IRQHandler(void)
{
  if(SdmmcTest == 1)
  {
  HAL_DMA_IRQHandler(uSdHandle.hdmatx);
  }
  else
  {
   HAL_DMA_IRQHandler(hAudioInTopRightFilter.hdmaReg);
  }
}

/**
  * @brief  This function handles DMA2 Stream 1 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_DFSDMx_DMAx_BUTTOM_LEFT_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hAudioInButtomLeftFilter.hdmaReg);
}

/**
  * @brief  This function handles DMA2 Stream 7 interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_DFSDMx_DMAx_BUTTOM_RIGHT_IRQHandler(void)
{
	// Check if hAudioInButtomRightFilter is initialized (P.B. 9.12.2022)
	if (hAudioInButtomRightFilter.State != HAL_DFSDM_FILTER_STATE_RESET) {
		HAL_DMA_IRQHandler(hAudioInButtomRightFilter.hdmaReg);
	}
	//check if handler is initialized (P.B. 9.12.2022):
	if (DMA2_UART1_Handle.State != HAL_DMA_STATE_RESET) {
		HAL_DMA_IRQHandler(&DMA2_UART1_Handle);
	}
}

/**
  * @brief  This function handles SDMMC2 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SDMMC_IRQHandler(void)
{
  HAL_SD_IRQHandler(&uSdHandle);
}



/**
* @brief  This function handles CAN1 RX0 interrupt request.
* @param  None
* @retval None
*/
void CAN1_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&CanHandle);
}


/* USER CODE END 1 */
