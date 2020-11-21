/*
 * ADC.h
 *
 * Created: 16/03/2019 17:22:27
 *  Author: Daniel
 */ 


#ifndef ADCC_H_
#define ADCC_H_

#include <asf.h>
#define MAX_ADC 4096
#define ADC_NUM_CHANNELS 3
#define __ADC_GET_CH2_FLOAT() (((float)ADCC_GlobalRawData[2]*3.33f)/((float)MAX_ADC-1.0f))

extern uint16_t ADCC_GlobalRawData[ADC_NUM_CHANNELS];

void ADCC_Init(void);


#endif /* ADC_H_ */