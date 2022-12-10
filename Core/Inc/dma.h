/*
 * dma.h
 *
 *  Created on: Dec 8, 2022
 *      Author: patriciobulic
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"


/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* DMA stream parameters definitions for M2M transfers between SRAM and SDRAM.
 *   You can modify these parameters to select
/*   a different DMA Stream and/or channel.                                   */
/* But note that only DMA2 Streams are capable of Memory to Memory transfers. */
#define DMA_SDRAM_INSTANCE               DMA2_Stream1
#define DMA_SDRAM_CHANNEL                DMA_CHANNEL_0
#define DMA_SRAM_INSTANCE_IRQ            DMA2_Stream1_IRQn
#define DMA_SDRAM_INSTANCE_IRQHANDLER    DMA2_Stream1_IRQHandler

#define DMA_UART1_INSTANCE               DMA2_Stream7
#define DMA_UART1_CHANNEL                DMA_CHANNEL_4
#define DMA_UART1_INSTANCE_IRQ           DMA2_Stream7_IRQn
#define DMA_UART1_INSTANCE_IRQHANDLER    DMA2_Stream7_IRQHandler
/* USER CODE END EC */

extern SDRAM_HandleTypeDef 			sdramHand;


HAL_StatusTypeDef DMA2_SDRAM_Config(DMA_HandleTypeDef* DmaHandle);
HAL_StatusTypeDef DMA2_UART1_Config(void);

#endif /* INC_DMA_H_ */
