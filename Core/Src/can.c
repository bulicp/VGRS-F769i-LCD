/*
 * can.c
 *
 *  Created on: Dec 23, 2022
 *      Author: patriciobulic
 */


/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "cmsis_os.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef     CanHandle;
//CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
extern uint8_t        TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;
extern osMessageQueueId_t 	  mid_MsgQueue;
static CAN_OBD2_MSGQUEUE_OBJ_t new_can_data;
static osStatus_t     osstatus;
/* Private function prototypes -----------------------------------------------*/



HAL_StatusTypeDef CAN1_Config(CAN_InitModeTypeDef mode)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CAN1;
  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  if (mode == CAN_NORMAL_MODE)
	  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  else if (mode == CAN_LOOPBACK_MODE)
	  CanHandle.Init.Mode = CAN_MODE_LOOPBACK;
  else if (mode == CAN_SILENTLOOP_MODE)
	  CanHandle.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  /*
   * P.B.
   * Timings: bit_t = (1 + BS1 + BS2)tq = 1/BIT_RATE
   * tq = PRE * tAPB1
   *
   * BS1 + BS2 = (1/BIT_RATE) / PRE * tAPB1  - 1
   *           = fAPB1 / BIT_RATE * PRE  -  1
   *
   * For fAPB1 = 50MHz, BIT_RATE = 500 kHz:
   * BS1 + BS2 = 100/PRE - 1
   * PRE = 5
   * BS1+BS2 = 20 - 1: BS1 = 13, BS2 = 6
   */
  CanHandle.Init.TimeSeg1 = CAN_BS1_13TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_6TQ;
  CanHandle.Init.Prescaler = 5;

  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    return HAL_ERROR;
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
#ifdef __OBD2__
  sFilterConfig.FilterIdHigh = 0x7E8<<5U;
#else
  sFilterConfig.FilterIdHigh = 0x0000; // no filtering, accept all
#endif
  sFilterConfig.FilterIdLow = 0x0000;
#ifdef __OBD2__
  sFilterConfig.FilterMaskIdHigh = 0x7F8<<5U;
#else
  sFilterConfig.FilterMaskIdHigh = 0x0000; // no filtering, accept all
#endif
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    return HAL_ERROR;
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    /* Start Error */
    return HAL_ERROR;
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    return HAL_ERROR;
  }

  return HAL_OK;
}



HAL_StatusTypeDef CAN_SendMessage(CAN_HandleTypeDef* CanHandle, CAN_TxHeaderTypeDef* TxHeader, uint8_t* pTxData){

	uint32_t              TxMailbox;

	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(CanHandle, TxHeader, pTxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		return HAL_ERROR;

	}
	/* Wait transmission complete */
	while(HAL_CAN_GetTxMailboxesFreeLevel(CanHandle) != 3) {}

	return HAL_OK;
}



HAL_StatusTypeDef CAN_ReceiveMessage(CAN_HandleTypeDef* CanHandle, CAN_RxHeaderTypeDef* RxHeader, uint8_t* pRxData){
	/* Start the Reception process */
	if(HAL_CAN_GetRxFifoFillLevel(CanHandle, CAN_RX_FIFO0) != 1)
	{
		/* Reception Missing */
		return HAL_ERROR;
	}

	if(HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, RxHeader, pRxData) != HAL_OK)
	{
		/* Reception Error */
		return HAL_ERROR;
	}

	/*
	if((RxHeader->StdId != 0x7DF)                    ||
	   (RxHeader->RTR != CAN_RTR_DATA)               ||
       (RxHeader->IDE != CAN_ID_STD)                 ||
       (RxHeader->DLC != 2)) {
		// Rx message Error
		return HAL_ERROR;
    }
	*/
    return HAL_OK; /* Test Passed */

}


/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
  }
  new_can_data.pid = RxData[1];
  for (int i=0; i<4;i++) {
    new_can_data.OBDData[i] = RxData[i+2];
  }
  new_can_data.OBDData[0] = 0x5B;

  /* PAZI!!!!! The highest interrupt priority that can be used by any interrupt service
  routine that makes calls to interrupt safe FreeRTOS API functions is
  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.
  DO NOT CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
  PRIORITY THAN THIS! (higher priorities are lower numeric values. */
  osstatus = osMessageQueuePut(mid_MsgQueue, &new_can_data, 0U, 0U); // Timeout should be set to zero if called from ISR!!
  if ( osstatus != osOK){
	  // Message queue error

  }



}


void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* CAN1 Periph clock enable */
  __HAL_RCC_CAN1_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* CAN1 TX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF9_CAN1;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* CAN1 RX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF9_CAN1;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*##-3- Configure the NVIC #################################################*/
  /* NVIC configuration for CAN1 Reception complete interrupt */

  /* The highest interrupt priority that can be used by any interrupt service
  routine that makes calls to interrupt safe FreeRTOS API functions is
  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.
  DO NOT CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
  PRIORITY THAN THIS! (higher priorities are lower numeric values. */
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1U, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
}



