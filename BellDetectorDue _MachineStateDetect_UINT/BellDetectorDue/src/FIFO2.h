/*
 * FIFO2.h
 *
 * Created: 22/08/2019 20:35:50
 *  Author: Daniel
 */ 


#ifndef FIFO2_H_
#define FIFO2_H_


typedef struct
{
	int OldestInd;
	float* Sound;
	float Sum;
	int len;
}XFifo2;

void FIFO2_Init(XFifo2* Fifo2, int len);
float FIFO2_Insert(XFifo2* Fifo2, float inp);
void FIFO2_Reset(XFifo2* Fifo2);
float FIFO2_GetNext(XFifo2* Fifo2);



#endif /* FIFO2_H_ */