/*
 * Timer.c
 *
 * Created: 15/03/2019 15:34:35
 *  Author: Daniel
 */ 
#include "Timer.h"
#include "Prints.h"

void Timer_init(void)
{
	pmc_enable_periph_clk(ID_TC0);
	pmc_enable_periph_clk(ID_TC1);
	tc_set_writeprotect(TC0,0);
	
	REG_PIOB_ABSR |= PIO_ABSR_P25;     // Switch the multiplexer to peripheral B for TIOA0
	REG_PIOB_PDR |= PIO_PDR_P25;        // Disable the GPIO on the corresponding pins
	tc_init(TC0,0,TC_CMR_WAVE|TC_CMR_WAVSEL_UP_RC|TC_CMR_ACPA_SET|TC_CMR_ACPC_CLEAR|TC_CMR_TCCLKS_TIMER_CLOCK2); // divide MCLK by 8 
	tc_init(TC0,1,TC_CMR_TCCLKS_TIMER_CLOCK4); // divide MCLK by 128
	TC0->TC_CHANNEL[0].TC_RC = 210;//238; // 210 cycles of MCLK/8 to reach 50Khz  -- 420 to reach 25KHz -- 230 to 45.6KHz~
	TC0->TC_CHANNEL[0].TC_RA = 105;//119; // 105 for 50Khz , 210 for 25kH
	NVIC_DisableIRQ(TC0_IRQn);
	//NVIC_EnableIRQ(TC0_IRQn);
	//tc_enable_interrupt(TC0,0,TC_IER_CPCS);
	tc_start(TC0,0);
	
}

void Timer_Sleep(uint32_t msDelay)
{
	tc_start(TC0,1);	
	//PrintLn("wait\n");
	while(TC0->TC_CHANNEL[1].TC_CV < 656*msDelay);
	//PrintLn("done wait\n");
	tc_stop(TC0,1);
}