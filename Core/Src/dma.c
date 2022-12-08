/*
 * dma.c
 *
 *  Created on: Dec 8, 2022
 *      Author: patriciobulic
 */

/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define __FIFO
#define __BURST_4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void SDRAM_TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void SDRAM_TransferError(DMA_HandleTypeDef *DmaHandle);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in dma.h file
  * @note  This function is used to :
  *        -1- Enable DMA2 clock
  *        -2- Select the DMA functional Parameters
  *        -3- Select the DMA instance to be used for the transfer
  *        -4- Initialize the DMA stream
  *        -5- Select Callbacks functions called after Transfer complete and
               Transfer error interrupt detection
  *        -6- Configure NVIC for DMA transfer complete/error interrupts
  *        -7- Start the DMA transfer using the interrupt mode
  * @param  None
  * @retval None
  */
HAL_StatusTypeDef DMA2_SDRAM_Config(DMA_HandleTypeDef* DmaHandle)
{
  /*## -1- Enable DMA2 clock #################################################*/
  __HAL_RCC_DMA2_CLK_ENABLE();

  /*##-2- Select the DMA functional Parameters ###############################*/
  DmaHandle->Init.Channel = DMA_SDRAM_CHANNEL;                     /* DMA_CHANNEL_0                    */
  DmaHandle->Init.Direction = DMA_MEMORY_TO_MEMORY;          /* M2M transfer mode                */
  DmaHandle->Init.PeriphInc = DMA_PINC_ENABLE;               /* Peripheral increment mode Enable */
  DmaHandle->Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable     */
  DmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* Peripheral data alignment : Word */
  DmaHandle->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    /* memory data alignment : Word     */
  DmaHandle->Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                  */
  DmaHandle->Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high            */
#ifdef __FIFO
  DmaHandle->Init.FIFOMode = DMA_FIFOMODE_ENABLE;            /* FIFO mode enabled                */
  DmaHandle->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;   /* FIFO threshold: full             */
#else
  DmaHandle->Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode enabled                */
#endif
#ifdef __BURST_4
  DmaHandle->Init.MemBurst = DMA_MBURST_INC4;                /* Memory burst                     */
  DmaHandle->Init.PeriphBurst = DMA_PBURST_INC4;             /* Peripheral burst                 */
#else
  DmaHandle->Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  DmaHandle->Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */
#endif

  /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
  DmaHandle->Instance = DMA_SDRAM_INSTANCE;


  /*##-4- Initialize the DMA stream ##########################################*/
  if (HAL_DMA_Init(DmaHandle) != HAL_OK)
  {
    /* Initialization Error */
    return HAL_ERROR;
  }

  /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
  HAL_DMA_RegisterCallback(DmaHandle, HAL_DMA_XFER_CPLT_CB_ID, SDRAM_TransferComplete);
  HAL_DMA_RegisterCallback(DmaHandle, HAL_DMA_XFER_ERROR_CB_ID, SDRAM_TransferError);

  /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriority(DMA_SRAM_INSTANCE_IRQ, 0, 0);

  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA_SRAM_INSTANCE_IRQ);

  return HAL_OK;
}

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void SDRAM_TransferComplete(DMA_HandleTypeDef *DmaHandle)
{
  //TODO
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void SDRAM_TransferError(DMA_HandleTypeDef *DmaHandle)
{
  //TODO
}
