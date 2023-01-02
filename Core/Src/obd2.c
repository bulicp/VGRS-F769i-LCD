/*
 * obd2.c
 *
 *  Created on: Dec 30, 2022
 *      Author: patriciobulic
 */


/* Includes ------------------------------------------------------------------*/
#include "obd2.h"
#include "can.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern CAN_HandleTypeDef     CanHandle;
CAN_TxHeaderTypeDef   TxHeader;
//CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
//uint8_t               RxData[8];

/* Private function prototypes -----------------------------------------------*/

HAL_StatusTypeDef OBD2_SendQuery(uint8_t iOBD2_ServiceMode, OBD2_Mode1_PID_TypeDef pid){
	/* Configure Transmission process */
	TxHeader.StdId = 0x7DF;			// broadcast ODP2 request to all ECUs present
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.DLC = 2;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Set the data to be transmitted */
	TxData[0] = iOBD2_ServiceMode;
	TxData[1] = pid;

	/* Start the CAN Transmission process */
	if (CAN_SendMessage(&CanHandle, &TxHeader, TxData) != HAL_OK){
		return HAL_ERROR;
	}

	return HAL_OK;
}



float OBD2DecodeEngineLoad(uint8_t *response)
{
	return ((response[0] * 100.0) / 255.0);
}

uint32_t OBD2DecodeEngineCoolantTemp(uint8_t *response)
{
	return (response[0] - 40);
}

uint32_t OBD2DecodeFuelPressure(uint8_t *response)
{
	return (response[0] * 3);
}

uint32_t OBD2DecodeIntakeAirTemp(uint8_t *response)
{
	return (response[0] - 40);
}

uint32_t OBD2DecodeEngineSpeed(uint8_t *response)
{
	return (((response[0]<<8) + response[1]) >> 2);
}

uint32_t OBD2DecodeVehicleSpeed(uint8_t *response)
{
	return response[0];
}

float OBD2DecodeMAFRate(uint8_t *response)
{
	return ((response[0]<<8) + response[1]) / 100.0;
}

float OBD2DecodeThrottlePosition(uint8_t *response)
{
	return (response[0] * 100.0) / 255.0;
}

float OBD2DecodeEngineFuelRate(uint8_t *response)
{
	return ((response[0]<<8) + response[1]) / 20.0;
}

uint32_t OBD2DecodeOilTemp(uint8_t *response)
{
	return (response[0] - 40);
}


void OBD2DecodeSupportedPIDs(uint8_t *response, OBD2_Supported_PIDs_TypeDef* supportedPIDS)
{
	supportedPIDS->SupportedPID_Engine_Load = (response[0] & 0x10U) ? 1 : 0;
	supportedPIDS->SupportedPID_Engine_Coolant_Temp = (response[0] & 0x10U) ? 1 : 0;

	supportedPIDS->SupportedPID_Fuel_Pressure = (response[1] & 0x40U) ? 1 : 0;
	supportedPIDS->SupportedPID_Engine_Speed = (response[1] & 0x10U) ? 1 : 0;
	supportedPIDS->SupportedPID_Vehicle_Speed = (response[1] & 0x08U) ? 1 : 0;
	supportedPIDS->SupportedPID_Intake_Air_Temp = (response[1] & 0x02U) ? 1 : 0;

	supportedPIDS->SupportedPID_Throttle_Position = (response[2] & 0x80U) ? 1 : 0;

}








