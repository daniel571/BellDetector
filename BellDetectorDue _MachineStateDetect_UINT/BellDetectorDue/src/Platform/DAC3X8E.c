/*
 * DAC3X8E.c
 *
 * Created: 16/01/2020 20:10:25
 *  Author: Daniel
 */ 
#include "DAC3X8E.h"
#include "sam3x8e.h"
#include "pmc.h"

void DAC3X8E_Init()
{
	uint32_t mr;
	pmc_enable_periph_clk(ID_DACC);
	
	//disable write protect
	DACC->DACC_CDR = 0;
	// Set trigger mode 1
	mr = DACC->DACC_MR & (~(DACC_MR_TRGSEL_Msk));
	DACC->DACC_MR = mr | DACC_MR_TRGEN_EN | DACC_MR_TRGSEL(1);
	
	// Set transfer mode 0 - half word mode
	DACC->DACC_MR &= (~DACC_MR_WORD_WORD);
	//Set flexible mode
	DACC->DACC_MR |= DACC_MR_TAG;
	// set timing
	mr = DACC->DACC_MR & (~(DACC_MR_REFRESH_Msk | DACC_MR_STARTUP_Msk));
	mr |= DACC_MR_REFRESH(1);
	mr &= ~DACC_MR_MAXS;
	mr |= (DACC_MR_STARTUP_Msk & ((3) << DACC_MR_STARTUP_Pos));
	DACC->DACC_MR = mr;
	//set power save off
	DACC->DACC_MR &= (~DACC_MR_SLEEP);
	DACC->DACC_MR |= DACC_MR_FASTWKUP;
	//disable INT
	DACC->DACC_IDR = 0xf;
	//enable ch0 and ch1
	DACC->DACC_CHER = DACC_CHER_CH0 << 0;
	DACC->DACC_CHER = DACC_CHER_CH0 << 1;
	//enable write protect
	DACC->DACC_CDR = 1;
	
	
}
void DAC3X8E_write(uint32_t ul_data)
{
	DACC->DACC_CDR = ul_data;
}