/*
 * obd2.h
 *
 *  Created on: Dec 30, 2022
 *      Author: patriciobulic
 */

#ifndef INC_OBD2_H_
#define INC_OBD2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"


typedef enum
{
  OBD2_PID_PIDS_SUPPORTED_01_20         = 0x00U,  // bit encoded PIDs supported [01 - 20]
  OBD2_PID_STATUS_SINCE_DTC             = 0x01U,
  OBD2_PID_FREEZE_DTC             		= 0x02U,
  OBD2_PID_FUEL_SYS_STATUS             	= 0x03U,
  OBD2_PID_ENGINE_LOAD             		= 0x04U,
  OBD2_PID_ENGINE_COOLANT_TEMP          = 0x05U,
  OBD2_PID_FUEL_PRESSURE				= 0X0AU,
  OBD2_PID_INTAKE_MANIFOLD_PRESSURE		= 0X0BU,
  OBD2_PID_ENGINE_SPEED					= 0X0CU,
  OBD2_PID_VEHICLE_SPEED				= 0X0DU,
  OBD2_PID_INTAKE_AIR_TEMP				= 0X0FU,
  OBD2_PID_THROTTLE_POSITION			= 0X11U,
  OBD2_PID_OXY_SENSOR_1					= 0X14U,
  OBD2_PID_OXY_SENSOR_2					= 0X15U,
  OBD2_PID_OXY_SENSOR_3					= 0X16U,
  OBD2_PID_OXY_SENSOR_4					= 0X17U,
  OBD2_PID_OXY_SENSOR_5					= 0X18U,
  OBD2_PID_OXY_SENSOR_6					= 0X19U,
  OBD2_PID_OXY_SENSOR_7					= 0X1AU,
  OBD2_PID_OXY_SENSOR_8					= 0X1BU,
  OBD2_PID_RUN_SINCE_ENGINE_START		= 0X1FU,
  OBD2_PID_PIDS_SUPPORTED_21_40         = 0x20U,
  OBD2_PID_FUEL_TANK_LEVEL_INPUT        = 0x2FU,
  OBD2_PID_DISTANCE_SINCE_CODES_CLEARED = 0x31U,
  OBD2_PID_ABS_BARO_PRESSURE 	        = 0x33U,
  OBD2_PID_PIDS_SUPPORTED_41_60         = 0x40U,
  OBD2_PID_FUEL_TYPE		            = 0x51U,
  OBD2_PID_ENGINE_OIL_TEMP	         	= 0x5CU,
  OBD2_PID_ENGINE_FUEL_RATE	         	= 0x5DU,
  OBD2_PID_PIDS_SUPPORTED_61_80         = 0x60U
} OBD2_Mode1_PID_TypeDef;


typedef struct {
   uint8_t SupportedPID_Engine_Load: 			1;
   uint8_t SupportedPID_Engine_Coolant_Temp: 	1;
   uint8_t SupportedPID_Fuel_Pressure: 		1;
   uint8_t SupportedPID_Engine_Speed: 			1;
   uint8_t SupportedPID_Vehicle_Speed: 			1;
   uint8_t SupportedPID_Intake_Air_Temp: 		1;
   uint8_t SupportedPID_Throttle_Position: 		1;
   uint8_t SupportedPID_FuelType: 				1;
   uint8_t SupportedPID_FuelRate: 				1;
} OBD2_Supported_PIDs_TypeDef;



HAL_StatusTypeDef OBD2_SendQuery(uint8_t iOBD2_ServiceMode, OBD2_Mode1_PID_TypeDef pid);

float OBD2DecodeEngineLoad(uint8_t *response);
uint32_t OBD2DecodeEngineCoolantTemp(uint8_t *response);
uint32_t OBD2DecodeFuelPressure(uint8_t *response);
uint32_t OBD2DecodeIntakeAirTemp(uint8_t *response);
uint32_t OBD2DecodeEngineSpeed(uint8_t *response);
uint32_t OBD2DecodeVehicleSpeed(uint8_t *response);
float OBD2DecodeMAFRate(uint8_t *response);
float OBD2DecodeThrottlePosition(uint8_t *response);
float OBD2DecodeEngineFuelRate(uint8_t *response);
uint32_t OBD2DecodeOilTemp(uint8_t *response);


#endif /* INC_OBD2_H_ */
