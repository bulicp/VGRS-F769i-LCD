/* Includes ------------------------------------------------------------------*/
#include "pa3SDRAM.h"

/* Private variables ---------------------------------------------------------*/
/* Buffer location should aligned to cache line size (32 bytes) */
ALIGN_32BYTES (uint32_t sdram_write_Buffer[SDRAM_BUFFER_SIZE]);   // buffer size = 1K (256*4)
ALIGN_32BYTES (uint32_t sdram_read_Buffer[SDRAM_BUFFER_SIZE]);

ALIGN_32BYTES (uint32_t matrixA[MAT_ROWS][SDRAM_COLS]);
ALIGN_32BYTES (uint32_t matrixB[MAT_ROWS][SDRAM_COLS]);




SDRAM_HandleTypeDef 			sdramHand;
static FMC_SDRAM_TimingTypeDef 	sdramTiming;
static FMC_SDRAM_CommandTypeDef sdramCmd;
extern DMA_HandleTypeDef    	DMA2_SDRAM_Handle;



/**
  * @brief  Initializes the SDRAM device.
  * @retval SDRAM status
  */
uint8_t Init_SDRAM(void)
{
  static uint8_t sdramstatus = SDRAM_ERROR;
  /* SDRAM device configuration */
  sdramHand.Instance = FMC_SDRAM_DEVICE;

  /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is up to 200Mhz) */
  /* These parameters are from the MT48LC4M32B2 Data Sheet, Table 18 and Table 19 */
  sdramTiming.LoadToActiveDelay    = 2;		// t_MRD (LMR to ACTIVE = 2 CLK)
  sdramTiming.ExitSelfRefreshDelay = 7;		// t_XSR (exit self refresh command to Activate = 70 ns)
  sdramTiming.SelfRefreshTime      = 5;		// t_RAS (ACTIVE-to-PRECHARGE = 42 ns)
  sdramTiming.RowCycleDelay        = 7;		// t_RC  (ACTIVE-to-ACTIVE = 70 ns)
  sdramTiming.WriteRecoveryTime    = 2;		// t_WR (WRITE-to-PRECHARGE = 1CLK+7ns)
  sdramTiming.RPDelay              = 2;		// t_RP  (20 ns @ 100MHz)
  sdramTiming.RCDDelay             = 2;		// t_RCD (20 ns @ 100MHz)


  sdramHand.Init.SDBank             = FMC_SDRAM_BANK1;				// use FMC SDRAM Bank 1
  sdramHand.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_8;	// 256 columns per row
  sdramHand.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;		// 4096 rows per bank
  sdramHand.Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;	    // 32-bit word
  sdramHand.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;	//
  sdramHand.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;		// CAS Latency
  sdramHand.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  sdramHand.Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;	    // SDRAM CLK is HCLK/2 (HCLK/3 also possible)
  sdramHand.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;		// single read requests are managed as
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	// bursts using FIFO. The SDRAM controller anticipates
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	    // the next read commands
  sdramHand.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

  /* SDRAM controller initialization */

  if(HAL_SDRAM_Init(&sdramHand, &sdramTiming) != HAL_OK)
  {
    sdramstatus = SDRAM_ERROR;
  }
  else
  {
    sdramstatus = SDRAM_OK;
  }

  /* Once the FMC SDRAM Ctrl is initialized, we can access and initialize the SDRAM chip */
  /* SDRAM initialization sequence */
  SDRAM_Initialization_sequence(SDRAM_REFRESH_COUNT);

  return sdramstatus;
}


/**
  * @brief  Init the SDRAM device.
  * SDRAMs must be initialized in a predefined manner. Operational procedures
  * other than those specified in the SDRAM Data Sheet may result in undefined operation.
  * @param  RefreshCount: SDRAM refresh counter value
  * @retval None
  */
void SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd = 0;

  /* Step 1: Configure a clock configuration enable command */
  sdramCmd.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  sdramCmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  sdramCmd.AutoRefreshNumber      = 1;
  sdramCmd.ModeRegisterDefinition = 0;


  /* Send the Clock Configuration Enable command to the target bank*/
  /* The command is sent as soon as the Command MODE field in the CMR is written */
  HAL_SDRAM_SendCommand(&sdramHand, &sdramCmd, SDRAM_TIMEOUT);

  /*
   * Once the clock is stable, the SDRAM requires a 100μs delay prior to issuing any command
   */

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);


  /*
   * Once the 100μs delay has been satisfied, a PRECHARGE command should be applied.
   * All banks must then be precharged, thereby placing the device in the all banks idle state.
   */
  /* Step 3: Configure a PALL (precharge all) command */
  sdramCmd.CommandMode            = FMC_SDRAM_CMD_PALL;
  sdramCmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  sdramCmd.AutoRefreshNumber      = 1;
  sdramCmd.ModeRegisterDefinition = 0;

  /* Send the Precharge All command to the target bank */
  /* The command is sent as soon as the Command MODE field in the CMR is written */
  HAL_SDRAM_SendCommand(&sdramHand, &sdramCmd, SDRAM_TIMEOUT);

  /*
   * Once in the idle state, at least two AUTO REFRESH cycles must be performed.
   * If desired, more than two AUTO REFRESH commands can be issued in the sequence.
   */
  /* Step 4: Configure an Auto Refresh command */
  sdramCmd.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  sdramCmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  sdramCmd.AutoRefreshNumber      = 2;
  sdramCmd.ModeRegisterDefinition = 0;

  /* Send the Auto-refresh commands to the target bank */
  /* The command is sent as soon as the Command MODE field in the CMR is written */
  HAL_SDRAM_SendCommand(&sdramHand, &sdramCmd, SDRAM_TIMEOUT);


  /*
   * The SDRAM is now ready for mode register programming. Because the mode register
   * will power up in an unknown state, it should be loaded with desired bit values prior to
   * applying any operational command. Using the LMR command, program the mode
   * register.
   */
  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |\
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
                     SDRAM_MODEREG_CAS_LATENCY_3           |\
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  sdramCmd.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  sdramCmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  sdramCmd.AutoRefreshNumber      = 1;
  sdramCmd.ModeRegisterDefinition = tmpmrd;

  /* Send the Load Mode Register command to the target bank */
  /* The command is sent as soon as the Command MODE field in the CMR is written */
  HAL_SDRAM_SendCommand(&sdramHand, &sdramCmd, SDRAM_TIMEOUT);

  /*
   * Wait at least tMRD time, during which only NOP or DESELECT commands are allowed.
   * This is automatically performed by FMC SDRAM controller
   * At this point the DRAM is ready for any valid command
   */

  /* Step 6: Set the refresh rate counter in Refresh Timer register */
  /* This 13-bit field defines the refresh rate of the SDRAM device. It is expressed in number of memory
     clock cycles. */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&sdramHand, RefreshCount);
}



void simple_SDRAM_test(void){

	// Init pisalni buffer:
	for (int i = 0; i<SDRAM_BUFFER_SIZE; i+=8) {
		sdram_write_Buffer[i] = 0x63336150;
		sdram_write_Buffer[i+1] = 0x6e206f69;
		sdram_write_Buffer[i+2] = 0x20616d69;
		sdram_write_Buffer[i+3] = 0x6d6a6f70;
		sdram_write_Buffer[i+4] = 0x21212161;
		sdram_write_Buffer[i+5] = 0x20202020;
		sdram_write_Buffer[i+6] = 0x20202020;
		sdram_write_Buffer[i+7] = 0x00202020;
	}

	for (int i = 0; i<SDRAM_BUFFER_SIZE; i++) {
		sdram_read_Buffer[i] = 0xffffffff;
	}

	// Piši v SDRAM:
	for (int i = 0; i < SDRAM_BUFFER_SIZE; i++)
	{
		*(uint32_t*) (SDRAM_DEVICE_ADDR + 4*i) = sdram_write_Buffer[i];
	}


	// Beri iz SDRAM-a:
	for (int i = 0; i < SDRAM_BUFFER_SIZE; i++)
	{
		sdram_read_Buffer[i] = *(uint32_t*) (SDRAM_DEVICE_ADDR + 4*i);
	}
}



void SDRAM_init_matrices(void) {
	for (int i = 0; i<MAT_ROWS; i++) {
		for(int j=0; j<SDRAM_COLS; j++ ){
			matrixA[i][j] = j;
		}
	}

	for (int i = 0; i<MAT_ROWS; i++) {
		for(int j=0; j<SDRAM_COLS; j++ ){
			matrixB[i][j] = 0;
		}
	}
}

uint8_t SDRAM_compare_matrices(void) {
	for (int i = 0; i<MAT_ROWS; i++) {
		for(int j=0; j<SDRAM_COLS; j++ ) {
			if (matrixB[i][j] != matrixA[i][j]) {
				return 1;
			}
		}
	}
	return 0;
}


void SDRAM_write_matrix(void){
	volatile uint32_t address;
	for (int i = 0; i<MAT_ROWS; i++) {
		for(int j=0; j<SDRAM_COLS; j++) {
			address = SDRAM_DEVICE_ADDR + ((i*SDRAM_COLS + j)<<2);
			*(uint32_t*)address = matrixA[i][j];
		}
	}
}

void SDRAM_mat_row_access_test(void){
	volatile uint32_t address;

#ifdef __ENABLE_WRITE__
	for (int k = 0; k < N; k++)
	{
		for (int i = 0; i<MAT_ROWS; i++) {
			for(int j=0; j<SDRAM_COLS; j++) {
				address = SDRAM_DEVICE_ADDR + ((i*SDRAM_COLS + j)<<2);
				*(uint32_t*)address = matrixA[i][j];
			}
		}
	}
#endif

	for (int k = 0; k < N; k++)
	{
		for (int i = 0; i<MAT_ROWS; i++) {
			for(int j=0; j<SDRAM_COLS; j++) {
				address = SDRAM_DEVICE_ADDR + ((i*SDRAM_COLS + j)<<2);
				matrixB[i][j] = *(uint32_t*)address;
			}
		}
	}
}


void SDRAM_DMA_mat_row_access_test(void){
	volatile uint32_t address;

	for (int k = 0; k < N; k++)
	{
		HAL_DMA_Start(&DMA2_SDRAM_Handle, (uint32_t) SDRAM_DEVICE_ADDR, (uint32_t) matrixB, MAT_ROWS * SDRAM_COLS);
		HAL_DMA_PollForTransfer(&DMA2_SDRAM_Handle, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	}
}



void SDRAM_mat_col_access_test(void){
	volatile uint32_t address;

#ifdef __ENABLE_WRITE__
	// N x Piši v SDRAM:
	for (int k = 0; k < N; k++)
	{
		for (uint32_t i = 0; i<SDRAM_COLS; i++) {
			for(uint32_t j=0; j<MAT_ROWS; j++) {
				address = SDRAM_DEVICE_ADDR + ((j*SDRAM_COLS + i)<<2);
				*(uint32_t*)address = matrixA[j][i];
			}
		}
	}
#endif

	for (int k = 0; k < N; k++)
	{
		for (int i = 0; i<SDRAM_COLS; i++) {
			for(int j=0; j<MAT_ROWS; j++) {
				address = SDRAM_DEVICE_ADDR + ((j*SDRAM_COLS + i)<<2);
				matrixB[j][i] = *(uint32_t*)address;
			}
		}
	}
}


