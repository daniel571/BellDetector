/*
 * FIFO.h
 *
 * Created: 08/05/2019 16:13:20
 *  Author: Daniel
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#define ALPHA 0.00000358034
#define BETA	6.586061962694726
#define BETA_SQRT 43.3762//  for 25kH, 43.383167 for 50KH
#define BETA_MUL2  13.1721// 14.935254 for 25kH, 13.17318 for 50KH
#define BETA_SQRT_INV 0.0231//0.0179322491973737 for 25kH  0.0230504149224514 for 50KH
#define BETA_SQRT_INV_Q 8192 // 13 bits
#define BETA_MUL2_Q 16 // 4 bits
#define InvFilterQ 0.00000762939453125

#include <stdint.h>
#include <stdio.h>
#include "Prints.h"



typedef struct  
{
	int WriteInx;
	int ReadInx;
	int WriteIndxY;
	int32_t Data[3];
	int32_t FilterY[3];
	int32_t Out;
	int32_t AlphaQ;
	int32_t BetaQ;
	int32_t BetaSQrtQ;
	int32_t BetaMul2Q;
	int32_t BetaSQRTInvQ;
}XFifo;

void FIFO_Init(XFifo* Fifo );

inline void FIFO_Insert(XFifo* Fifo, int32_t input)
{
	Fifo->Data[Fifo->WriteInx] = input;
	Fifo->WriteInx = Fifo->WriteInx + 1;
	if(Fifo->WriteInx == 3)
		Fifo->WriteInx = 0;
}

inline void FIFO_YInsert(XFifo* Fifo, int32_t input)
{
	Fifo->FilterY[Fifo->WriteIndxY] = input;
	Fifo->WriteIndxY = Fifo->WriteIndxY + 1;
	if(Fifo->WriteIndxY == 3)
		Fifo->WriteIndxY = 0;
}
/*
double FIFO_Read(XFifo* Fifo)
{
	double RetVal;
	RetVal = Fifo->Data[Fifo->ReadInx];
	Fifo->ReadInx = Fifo->ReadInx + 1;
	if(Fifo->ReadInx>3)
		Fifo->ReadInx = 0;
	return RetVal;
}*/

inline int32_t FIFO_FilterInsert(XFifo* Fifo,int32_t Xn,int32_t Xnp,int32_t Xnpp)
{
	int32_t Out,Yp,Ypp;
	//float OutF;
	//char f[60];
	switch(Fifo->WriteIndxY)
	{
		case 0:
		Yp = Fifo->FilterY[2];
		Ypp = Fifo->FilterY[1];
		break;
		case 1:
		Yp = Fifo->FilterY[0];
		Ypp = Fifo->FilterY[2];
		break;
		default: //2
		Yp = Fifo->FilterY[1];
		Ypp = Fifo->FilterY[0];
		break;
	}
	Out = (Xn*BETA_MUL2_Q - 2*BETA_MUL2_Q*Xnp + BETA_MUL2_Q*Xnpp + Fifo->BetaMul2Q*Yp - Ypp*BETA_MUL2_Q)*(Fifo->BetaSQRTInvQ); // The second order filter HPF*/
	//Out = Out>>(4+13);
	//OutF = (float)Out * InvFilterQ;
	//sprintf(f,"X1 X2 X3=%ld %ld %ld out=%ld b=%ld outf %f\n", Xn, Xnp,Xnpp,Out,Fifo->BetaSQRTInvQ,OutF);
	//PrintLn(f);
	FIFO_YInsert(Fifo, (Out>>(4+13)) );
	return Out;
}


inline int32_t FIFO_Filter(XFifo* Fifo)
{
	int32_t Xn,Xnp,Xnpp;	
	switch(Fifo->WriteInx)
	{
		case 0:
		Xn = Fifo->Data[2];
		Xnp = Fifo->Data[1];
		Xnpp = Fifo->Data[0];
		break;
		case 1:
		Xn = Fifo->Data[0];
		Xnp = Fifo->Data[2];
		Xnpp = Fifo->Data[1];
		break;
		default: // 2
		Xn = Fifo->Data[1];
		Xnp = Fifo->Data[0];
		Xnpp = Fifo->Data[2];
		break;
	}
	return (FIFO_FilterInsert(Fifo, Xn, Xnp, Xnpp));
	 
	
}

#endif /* FIFO_H_ */