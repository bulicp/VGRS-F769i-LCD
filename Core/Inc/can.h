/*
 * can.h
 *
 *  Created on: Dec 23, 2022
 *      Author: patriciobulic
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_


/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"


typedef enum
{
  CAN_NORMAL_MODE             = 0x00U,  // Normal mode
  CAN_LOOPBACK_MODE,  					// Loopback mode
  CAN_SILENTLOOP_MODE,  				// Silent combined with loop-back
  CAN_SILENT_MODE   					// Silent mode
} CAN_InitModeTypeDef;

typedef struct {                                // object data type
  uint8_t pid;
  uint8_t OBDData[4];
} CAN_OBD2_MSGQUEUE_OBJ_t;



HAL_StatusTypeDef CAN1_Config(CAN_InitModeTypeDef mode);
HAL_StatusTypeDef CAN_SendMessage(CAN_HandleTypeDef* CanHandle, CAN_TxHeaderTypeDef* TxHeader, uint8_t* pTxData);
HAL_StatusTypeDef CAN_ReceiveMessage(CAN_HandleTypeDef* CanHandle, CAN_RxHeaderTypeDef* RxHeader, uint8_t* pRxData);

//#define __OBD2__


#endif /* INC_CAN_H_ */
