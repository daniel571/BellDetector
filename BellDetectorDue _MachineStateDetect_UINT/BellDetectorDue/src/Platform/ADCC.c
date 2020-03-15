/*
 * ADC.c
 *
 * Created: 16/03/2019 17:22:12
 *  Author: Daniel
 */ 
#include "ADCC.h"

void ADCC_Init()
{
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(),sysclk_get_main_hz()/4, ADC_STARTUP_TIME_5);
	adc_configure_timing(ADC,20,ADC_SETTLING_TIME_0,50);
	adc_configure_trigger(ADC,ADC_TRIG_TIO_CH_0,0);
	adc_disable_all_channel(ADC);
	adc_disable_interrupt(ADC, 0xFFFFFFFF);
	adc_set_resolution(ADC, ADC_12_BITS);
	adc_enable_channel(ADC,ADC_CHANNEL_0);
	adc_enable_channel(ADC,ADC_CHANNEL_1);
	adc_enable_interrupt(ADC,ADC_IER_EOC0);
	NVIC_EnableIRQ(ADC_IRQn);	
	adc_start(ADC);
}