/*
 * uart.c
 *
 *  Created on: Dec 6, 2022
 *      Author: patriciobulic
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

HAL_StatusTypeDef USART1_Init(UART_HandleTypeDef* huart1)
{

  huart1->Instance = USART1;
  huart1->Init.BaudRate = 115200;
  huart1->Init.WordLength = UART_WORDLENGTH_8B;
  huart1->Init.StopBits = UART_STOPBITS_1;
  huart1->Init.Parity = UART_PARITY_NONE;
  huart1->Init.Mode = UART_MODE_TX_RX;
  huart1->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  //huart1->Init.OverSampling = UART_OVERSAMPLING_16;
  //huart1->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  //huart1->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(huart1) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

