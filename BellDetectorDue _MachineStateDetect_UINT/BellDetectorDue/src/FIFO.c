/*
 * FIFO.c
 *
 * Created: 08/05/2019 16:13:02
 *  Author: Daniel
 */ 
#include "FIFO.h"

int32_t AlphaQ,BetaQ,BetaSQrtQ,BetaMul2Q,BetaSQRTInvQ;

void FIFO_Init(XFifo* Fifo1 )
{
	Fifo1->WriteInx = 0;
	Fifo1->ReadInx = 0;
	Fifo1->Data[0] = 0;
	Fifo1->Data[1] = 0;
	Fifo1->Data[2] = 0;
	Fifo1->FilterY[0] = 0;
	Fifo1->FilterY[1] = 0;
	Fifo1->FilterY[2] = 0;
	Fifo1->WriteIndxY = 0;
	
	Fifo1->AlphaQ = (int32_t)(ALPHA * BETA_MUL2_Q);
	Fifo1->BetaQ =  (int32_t)(BETA * BETA_MUL2_Q);
	Fifo1->BetaSQrtQ = (int32_t)(BETA_SQRT * BETA_MUL2_Q);
	Fifo1->BetaMul2Q =  (int32_t)(BETA_MUL2 * BETA_MUL2_Q);
	Fifo1->BetaSQRTInvQ =  (int32_t)(BETA_SQRT_INV * BETA_SQRT_INV_Q);
}
