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
// len = T (integration time)
void FIFO2_Init(XFifo2* Fifo2, int len)
{
	int i;
	Fifo2->OldestInd = 0;
	Fifo2->len = len;
	Fifo2->Wave = (uint32_t*)malloc(sizeof(uint32_t)*len);
	
	for (i=0;i<len;i++)
	{
		Fifo2->Wave[i] = 0;	
	}
	Fifo2->Energy = 0;
	
}


/*
Insert new digital wave strength
Input: the Fifo2 , inp=new strength
Ret : the Energy of total vector upon T
*/
uint32_t FIFO2_Insert(XFifo2* Fifo2, uint32_t inp)
{
	Fifo2->Energy += (inp*inp - Fifo2->Wave[Fifo2->OldestInd]*Fifo2->Wave[Fifo2->OldestInd]);	
	Fifo2->Wave[Fifo2->OldestInd] = inp;
	if(Fifo2->OldestInd == (Fifo2->len -1))	
		Fifo2->OldestInd = 0;	
	else
		Fifo2->OldestInd++;
	return Fifo2->Energy;
}

void FIFO2_Reset(XFifo2* Fifo2)
{
	int i;
	Fifo2->OldestInd = 0;	
	
	for (i=0;i<Fifo2->len;i++)
	{
		Fifo2->Wave[i] = 0;
	}
	Fifo2->Energy = 0;
	
}
/*
uint32_t FIFO2_GetNext(XFifo2* Fifo2)
{
	uint32_t RetVal = Fifo2->Wave[Fifo2->OldestInd];
	if(Fifo2->OldestInd == (Fifo2->len -1))
		Fifo2->OldestInd = 0;
	else
		Fifo2->OldestInd++;
	return RetVal;
}*/
/*void FIFO2_Init(XFifo2* Fifo2)
{
int i;
Fifo2->OldestInd = 0;
for (i=0;i<10000;i++)
{
Fifo2->Wave[i] = 0.0;
}
Fifo2->Energy = 0;

}*/