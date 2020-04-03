/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <stdio.h>
#include "Timer.h"
#include "ADCC.h"
#include "SPIC.h"
#include "RF24.h"
#include "Prints.h"
#include "FIFO.h"
#include "IntFLASH.h"
#include "FIFO2.h"
#include "DAC3X8E.h"
#include <math.h>

enum XDetectorState{
	REGULAR_Detector = 0,
	CALIBRATE_Detector,
	UNKNOWN_Detector
};

int32_t Data;
uint32_t Calibration;
uint32_t Threshold= 4000.0;
uint32_t TTest = 4000;
int32_t OutData = 0.0;
float AudioFilteredIntg = 0.0;
float AudioFilteredRMS = 0.0;
float SoundSum = 0.0;
int INTEG_index = 0;
int Det_flag = 0;
float Maximum =0.0;
float debug_Arr[25] = {0};
int32_t Data_arr[25] = {0};
enum XDetectorState DetectorState;
enum XDetectorState PrevDetectorState;
float Fifo2LenAverage;
uint32_t delay;
uint32_t FilterState = 6;
int32_t FirstDetVal,SecondDetVal;
uint32_t MaxEnergy;
uint32_t MaxAllowedFirstVal = UINT32_MAX;

#define MAX_CALIBRATION 6//11.0
#define MAX_SIGNAL		9
#define FIFO2_LEN 1024

uint16_t outp[2] = {0xff11,0xff90};
XFifo Fifo;

XFifo2 WaveArray;
XFifo2 Fifo2p2;
XFifo2 fifoDebug;

uint GetDetectState(void);
void Detector_Calib2Regular(void);

uint GetDetectState()
{
	uint32_t inputpin;
	uint RetVal;
	inputpin = pio_get_pin_value(PIO_PB26_IDX);
	switch(inputpin)
	{
		case REGULAR_Detector:
		RetVal = REGULAR_Detector;
		break;
		case CALIBRATE_Detector:
		RetVal = CALIBRATE_Detector;
		break;
		default:
		RetVal = UNKNOWN_Detector;
		break;
	}
	return RetVal;
}

void Detector_Calib2Regular()
{
	// stop ADC channel 1 
	// save Calibration in FLASH (NFC)	
	Byte b2[10];
	FilterState = 6;
	memcpy(b2,&Threshold,sizeof(uint32_t));
	//memcpy(b2,&TTest,sizeof(uint32_t));
	WriteCalibToFlash(b2);
	PrintLn("Write to flash\n");
//	adc_disable_interrupt(ADC,ADC_IER_EOC0);	
	
}

/*void Detector_Reg2Calib()
{
	// start ADC Channel 1 
	// Update Calibration according to potentiometer ADC ch1	
}*/

int main (void)
{
	#if DEBUG_UART 
	const sam_uart_opt_t uart1Settings = {sysclk_get_cpu_hz(), 115200UL, UART_MR_PAR_NO};
	#endif
	char out_str[70] = {0};
	int i;	


	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();

	#if DEBUG_UART 
	//Allow UART to control PB2 and PB3
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PINS_UART);
	//Enable UART1 Clock	
	sysclk_enable_peripheral_clock(ID_UART);
	#endif
	gpio_configure_group(PIOB, PIO_PB26, PIO_INPUT|PIO_DEBOUNCE); // arduino due pin # 22
	gpio_configure_group(PIOD, PIO_PD1, PIO_OUTPUT_1); // arduino due pin # 26 OUTPUT red LED error
	gpio_configure_group(PIOB, PIO_PB21, PIO_OUTPUT_1); // arduino Blue LED
	gpio_configure_group(PIOA, PIO_PA7, PIO_OUTPUT_0); // arduino RT meas
	gpio_configure_group(PIOA, PIO_PA15, PIO_INPUT|PIO_DEBOUNCE); // arduino due pin # 24 BUTTON
	// might conister to use pull down instead of debouncer
	board_init();
	pio_set_pin_low(LED2_GPIO);	 // Green LED
	pio_set_pin_high(PIO_PD1_IDX); // Red LED
	pio_set_pin_high(PIO_PB21_IDX); // Blue LED
	pio_set_pin_low(PIO_PA7_IDX); 
	
	
	// Init FIFO's
	FIFO_Init(&Fifo);
	FIFO2_Init(&WaveArray,FIFO2_LEN);
	//Fifo2LenAverage = 1.0f/((float)Fifo2p1.len);
	//FIFO2_Init(&Fifo2p2,5435);
	FIFO2_Init(&fifoDebug,3000);
		
	
	Timer_init();
	
	SPI_Init();
	FLASH_Init();
	
	#if DEBUG_UART
	uart_init(UART, &uart1Settings);
	//irq_register_handler(UART_IRQn, 0);
	#endif
	
	RF_InitTX();
	for(i=0;i<0xff;i++)
		__NOP;
	RF_SendBell(RF24_TEST);
	pio_set_pin_high(LED2_GPIO);
	pio_set_pin_low(PIO_PD1_IDX);
	pio_set_pin_low(PIO_PB21_IDX);
	Timer_Sleep(1000);
	pio_set_pin_low(LED2_GPIO);
	pio_set_pin_high(PIO_PD1_IDX);
	pio_set_pin_high(PIO_PB21_IDX);
	
	//Timer_Sleep(10000);
	DetectorState = GetDetectState();
	PrevDetectorState = DetectorState;

	if(DetectorState == REGULAR_Detector)
	{
		Byte *p;
		p = ReadCalibFromFlash();
		memcpy(&Threshold,p,sizeof(uint32_t));
	}
		
	ADCC_Init();
	DAC3X8E_Init();
	
	/*
	Insert application code here, after the board has been initialized. */
	while(1)
	{	
		//Calibration = ADC->ADC_CDR[1];		
		Calibration = ADCC_GlobalRawData[1];
		DetectorState = GetDetectState();
		if((DetectorState != UNKNOWN_Detector) &&(DetectorState != PrevDetectorState)) // State changed
		{
			PrintLn("State change\n");
			switch(DetectorState)
			{
				case REGULAR_Detector:				
				Detector_Calib2Regular();
				break;
				case CALIBRATE_Detector:
				FilterState = 6;				
				break;
				default:
				Detector_Calib2Regular();
			}	
			PrevDetectorState = DetectorState;
		}
		
		switch(DetectorState)
		{
			case REGULAR_Detector:
			MaxAllowedFirstVal = Threshold + (4<<17);//(29<<17)/10;
			break;
			case CALIBRATE_Detector:
			pio_set_pin_high(PIO_PD1_IDX);
			MaxAllowedFirstVal = UINT32_MAX;
			break;
			default:
			break;
		}
		if(FilterState == 5)
			pio_set_pin_low(PIO_PD1_IDX);
		else
			pio_set_pin_high(PIO_PD1_IDX);
				
		//sprintf(out_str,"%lu\n", inputpin);	
		//Echo the received byte
			
		// We need to define a threshold and if OutData is greater than this threshold we send an RF signal to indicate the user that the door bell was rang by visitor
		// According to my experience 50.0 is a fair threshold to define 
		
		if(Det_flag)
		{		
			Det_flag = 0;				
			
			switch (DetectorState)
			{
				case REGULAR_Detector:				
				for(i=0;i<10;i++)
				{
					//PrintLn("REGULAR_Detector\n");
					Timer_Sleep(20);				
					RF_SendBell(RF24_BELLDOOR);
				}
				//debug code
				/*for(i=0;i<fifoDebug.len;i++)
				{
					sprintf(out_str,"%f\n", FIFO2_GetNext(&fifoDebug));
					PrintLn(out_str);
				}*/
				pio_set_pin_high(LED2_GPIO);
				Timer_Sleep(500);
				pio_set_pin_low(LED2_GPIO);
				break;
				case CALIBRATE_Detector:
				pio_set_pin_high(LED2_GPIO);
				if((FirstDetVal>(Threshold+(1<<17))) && (FirstDetVal<(Threshold+(3<<17))))
					pio_set_pin_low(PIO_PB21_IDX);
				Timer_Sleep(3000);				
				pio_set_pin_low(LED2_GPIO);
				pio_set_pin_high(PIO_PB21_IDX);
				//PrintLn("CALIBRATE_Detector\n");
				break;
				default:
				break;
			}			
				

			FIFO_Init(&Fifo);
			FIFO2_Reset(&WaveArray);
			FilterState = 6;
			delay = 0;
			//FIFO2_Reset(&Fifo2p2);
			Timer_Sleep(20);
			ADCC_Init();
		}
		if(DetectorState == CALIBRATE_Detector){
			Threshold = Calibration*(1<<5)*MAX_CALIBRATION;
			//sprintf(out_str,"Th %f\n",Threshold);
			//PrintLn(out_str);
		}
	
		#if DEBUG_UART
		Timer_Sleep(10);
		sprintf(out_str,"%lu\r\n", ADCC_GlobalRawData[2]);//ADC->ADC_CDR[2]);
		PrintLn(out_str);		
		#endif
		Timer_Sleep(100);
	
		
	}
}

__no_inline
RAMFUNC
ISR(ADC_Handler)
{	
	if(ADC->ADC_ISR & ADC_IER_ENDRX)
	{		
		//float Inp;
		//pio_set_pin_high(LED2_GPIO); //for real time debug	
		uint32_t OutDataToDAC0;
		uint32_t OutDataToDAC1;
	//	uint32_t Energy;
		pio_set_pin_high(PIO_PA7_IDX);
		
		ADC->ADC_RNPR =  (uint32_t)ADCC_GlobalRawData;
		ADC->ADC_RNCR = ADC_NUM_CHANNELS;
		Data = ADCC_GlobalRawData[0] - MAX_ADC/2;;
		//Data = ADC->ADC_CDR[0]-MAX_ADC/2;
		
		//Energy = FIFO2_Insert(&WaveArray, Data);
		FIFO_Insert(&Fifo,Data);
		OutData = FIFO_Filter(&Fifo);
		OutData = abs(OutData);
		//SoundSum = FIFO2_Insert(&Fifo2p1, OutData);		
		//FIFO2_Insert(&fifoDebug,SoundSum);
		switch(FilterState)		
		{
			case 0:
				SecondDetVal = 0;
				FirstDetVal = 0;
				//MaxEnergy = 0;
				if(OutData>MaxAllowedFirstVal)
				{
					FilterState = 5;
					delay = 0;
				}
				else if(OutData>Threshold)
				{
					//Det_flag = 1;
					//adc_disable_interrupt(ADC,ADC_IER_EOC0);;
					FirstDetVal = OutData;
					//MaxEnergy = Energy;
					FilterState = 1;
					delay = 0;					
				}	
			break;
			
			case 1:
				if(OutData>MaxAllowedFirstVal)
				{
					FilterState = 5;
					delay = 0;
				}
				else
				{
					if(FirstDetVal<(OutData))
					{					
						FirstDetVal	= OutData; // finding the max FirstDetVal					
					}				
					delay++;
					if(delay>=2600)
					{
						FilterState = 2;
						delay = 0;
					}
				}
				/*if(Energy > MaxEnergy)
					MaxEnergy = Energy;*/
			break;
				
			case 2:
				if(OutData>FirstDetVal)
				{
					FilterState = 5;
					delay = 0;
				}				
				else if(delay<9000)
				{
					delay++;
				}
				else
				{
					FilterState = 3;
					delay = 0;
				}				
			break;
			
			case 3:
				if(delay<10000)
				{
					delay++;
					if(SecondDetVal<OutData)					
						SecondDetVal = OutData;
				}
				else
				{
					if( ((SecondDetVal/5)<=(FirstDetVal/2)) && ((SecondDetVal/126)>=(FirstDetVal/100)) )  //last working value = 1.25
					{
						FilterState = 4;
						delay = 0;
					}
					else
					{
						FilterState = 5;
						delay = 0;
					}
				}				
			break;
			
			case 4:
				if(delay<250)
				{
					delay++;					
				}
				else
				{
					Det_flag = 1;
					FilterState = 6;
					delay = 0;
					adc_disable_interrupt(ADC,ADC_IER_ENDRX);
					adc_reset(ADC);
				}				
			break;
			
			case 5: // Noise state
				if(delay<150000) //3 seconds hold off due noise
				{					
					delay++;
				}
				else
				{					
					FilterState = 0;
					delay = 0;
				}			
			break;
			
			case 6:
				if(delay<50000) //1 seconds to stable the filter
				{
					delay++;
				}
				else
				{
					FilterState = 0;
					delay = 0;
				}
				break;
			default:break;
			
		}	
		//pio_set_pin_low(LED2_GPIO);// for real time debug
		if(!pio_get_pin_value(PIO_PA15_IDX))
		{
			OutDataToDAC0  = (FirstDetVal>>9)&0xfff;			
			OutDataToDAC1  = ((SecondDetVal>>9)&0xfff)|(1<<12);			
			//OutDataToDAC1 = ((MaxEnergy >> 18)&0xfff)|(1<<12);
		}
		else
		{
			OutDataToDAC0 = MAX_SIGNAL<<(17-9);
			OutDataToDAC1  = ((MAX_SIGNAL)<<(17-9))|(1<<12);
			//OutDataToDAC1  = (1<<(28-18))|(1<<12);
		}
		DAC3X8E_write(OutDataToDAC0);
		DAC3X8E_write(OutDataToDAC1);
		pio_set_pin_low(PIO_PA7_IDX);
	}
}

