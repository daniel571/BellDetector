/*
 * DAC3X8E.h
 *
 * Created: 16/01/2020 20:10:34
 *  Author: Daniel
 */ 


#ifndef DAC3X8E_H_
#define DAC3X8E_H_
#include <stdint.h>

void DAC3X8E_Init(void);
void DAC3X8E_write(uint32_t ul_data);


#endif /* DAC3X8E_H_ */