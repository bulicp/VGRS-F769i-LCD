/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PA3_SDRAM_H
#define __PA3_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"




/* Private define ------------------------------------------------------------*/
#define SDRAM_BUFFER_SIZE        ((uint32_t)0x0100)   		// 256
#define MAT_ROWS			     ((uint32_t)64)
//#define N 150
#define N 2

/**
  * @brief  SDRAM status structure definition
  */
#define   SDRAM_OK         ((uint8_t)0x00)
#define   SDRAM_ERROR      ((uint8_t)0x01)

/** @defgroup STM32F769I_DISCOVERY_SDRAM_Exported_Constants SDRAM Exported Constants
  * @{
  */


/*
 * LCD Frame Buffer starts at 0xC0000000
 *
 */

// Bank 0 is ussed by LTDC controller (framebuffer), so it is recommended to
// use another bank for general SDRAM access.
// 128 Mbit 4-bank SDRAM = 16MB 4-bank SDRAM, 4 MB per bank, PB, 2022
// 4 MB = 4 * 16^5 = 4*2^20

#define PA3_SDRAM_DEVICE_ADDR_RW  ((uint32_t)(0xC0000000) + 0x00400000) // 4MB offset
#define SDRAM_DEVICE_SIZE  ((uint32_t)0x1000000)  /* SDRAM device size in MBytes, 16 MB */



#define SDRAM_ROWS						 4096
#define SDRAM_COLS						 256


/*
 * SDRAM Refresha rate: 64 ms, 4096 rows: 15.625 us/row
 * SDRAM clk = 10 ns, refresh count = 15.625us/10ns = 1562
 * Refresh count = 1562 - 22 (safe margin) = 1540
 */
#define SDRAM_REFRESH_COUNT              ((uint32_t)1540)   /* SDRAM refresh counter (100Mhz SDRAM clock) */

#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/** @defgroup SDRAM Exported Functions
  * @{
  */
uint8_t Init_SDRAM(void);
void SDRAM_Initialization_sequence(uint32_t RefreshCount);
void simple_SDRAM_test(void);


void SDRAM_init_matrices(void);
void SDRAM_write_matrix(void);
uint8_t SDRAM_compare_matrices(void);
void SDRAM_mat_row_access_test(void);
void SDRAM_mat_col_access_test(void);
void SDRAM_DMA_mat_row_access_test(void);



#endif

