/*
 * FIFO2.h
 *
 * Created: 22/08/2019 20:35:50
 *  Author: Daniel
 */ 


#ifndef FIFO2_H_
#define FIFO2_H_

#include <stdint.h>
#if 0

typedef struct
{
	int OldestInd;
	uint32_t* Wave;
	uint32_t Energy;
	int len;
}XFifo2;

void FIFO2_Init(XFifo2* Fifo2, int len);
uint32_t FIFO2_Insert(XFifo2* Fifo2, uint32_t inp);
void FIFO2_Reset(XFifo2* Fifo2);
//uint32_t FIFO2_GetNext(XFifo2* Fifo2);

#endif

#endif /* FIFO2_H_ */

