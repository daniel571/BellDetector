/*
 * FIFO2.c
 *
 * Created: 22/08/2019 20:35:38
 *  Author: Daniel
 */ 
#include "FIFO2.h"
#include <malloc.h>


// Fifo2p1 located at 0x20072de0 after malloc - stack section end in 20072dd8 (8 bytes dif)
// Fifo2p2 located at 0x20077438 after malloc
void FIFO2_Init(XFifo2* Fifo2, int len)
{
	int i;
	Fifo2->OldestInd = 0;
	Fifo2->len = len;
	Fifo2->Sound = (float*)malloc(sizeof(float)*len);
	
	for (i=0;i<len;i++)
	{
		Fifo2->Sound[i] = 0.0;	
	}
	Fifo2->Sum = 0;
	
}

float FIFO2_Insert(XFifo2* Fifo2, float inp)
{
	Fifo2->Sum -= Fifo2->Sound[Fifo2->OldestInd];
	Fifo2->Sum += inp;
	Fifo2->Sound[Fifo2->OldestInd] = inp;
	if(Fifo2->OldestInd == (Fifo2->len -1))	
		Fifo2->OldestInd = 0;	
	else
		Fifo2->OldestInd++;
	return Fifo2->Sum;
}

void FIFO2_Reset(XFifo2* Fifo2)
{
	int i;
	Fifo2->OldestInd = 0;	
	
	for (i=0;i<Fifo2->len;i++)
	{
		Fifo2->Sound[i] = 0.0;
	}
	Fifo2->Sum = 0;
	
}

float FIFO2_GetNext(XFifo2* Fifo2)
{
	float RetVal = Fifo2->Sound[Fifo2->OldestInd];
	if(Fifo2->OldestInd == (Fifo2->len -1))
		Fifo2->OldestInd = 0;
	else
		Fifo2->OldestInd++;
	return RetVal;
}
/*void FIFO2_Init(XFifo2* Fifo2)
{
int i;
Fifo2->OldestInd = 0;
for (i=0;i<10000;i++)
{
Fifo2->Sound[i] = 0.0;
}
Fifo2->Sum = 0;

}*/