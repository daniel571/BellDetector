/*
 * Prints.c
 *
 * Created: 27/04/2019 21:45:44
 *  Author: Daniel
 */ 

#include "Prints.h"
#include <asf.h>

void PrintLn(const char *str)
{
	#if DEBUG_UART
	int i=0;
	while(str[i]!='\n')
	{
		while (!(UART->UART_SR & UART_SR_TXRDY));
		uart_write(UART, str[i]);
		i++;		
	}
	while (!(UART->UART_SR & UART_SR_TXRDY));
	uart_write(UART, str[i]);
	#endif
}