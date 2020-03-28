/*
 * ADC.c
 *
 * Created: 16/03/2019 17:22:12
 *  Author: Daniel
 */ 
#include "ADCC.h"

uint16_t ADCC_GlobalRawData[ADC_NUM_CHANNELS];

void ADCC_Init()
{
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(),sysclk_get_main_hz()/4, ADC_STARTUP_TIME_5);
	adc_configure_timing(ADC,20,ADC_SETTLING_TIME_0,50);
	adc_configure_trigger(ADC,ADC_TRIG_TIO_CH_0,0);
	adc_disable_all_channel(ADC);
	adc_disable_interrupt(ADC, 0xFFFFFFFF);
	adc_set_resolution(ADC, ADC_12_BITS);
	ADC->ADC_PTCR =  ADC_PTCR_RXTEN;
	ADC->ADC_RPR = (uint32_t)ADCC_GlobalRawData;
	ADC->ADC_RCR = ADC_NUM_CHANNELS;
	ADC->ADC_RNPR =  (uint32_t)ADCC_GlobalRawData;
	ADC->ADC_RNCR = ADC_NUM_CHANNELS;
	adc_enable_channel(ADC,ADC_CHANNEL_0);
	adc_enable_channel(ADC,ADC_CHANNEL_1);
	adc_enable_channel(ADC,ADC_CHANNEL_2);
	//adc_enable_interrupt(ADC,ADC_IER_EOC0);
	adc_enable_interrupt(ADC,ADC_IER_ENDRX);
	NVIC_EnableIRQ(ADC_IRQn);	
	adc_start(ADC);
}