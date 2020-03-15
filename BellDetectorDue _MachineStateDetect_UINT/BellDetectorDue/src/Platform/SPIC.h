/*
 * SPIC.h
 *
 * Created: 12/04/2019 23:36:31
 *  Author: Daniel
 */ 


#ifndef SPIC_H_
#define SPIC_H_

#include <asf.h>
#define DEFAULT_CHIP_ID 0

void SPI_Init(void);
uint16_t SPIC_Write(uint16_t *data, int length, uint16_t *SR);
uint16_t SPIC_Read(uint16_t *data, int length, uint16_t *SR, uint16_t *Resp);


#endif /* SPIC_H_ */