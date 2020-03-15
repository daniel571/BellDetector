/*
 * SPIC.c
 *
 * Created: 12/04/2019 23:35:39
 *  Author: Daniel
 */ 
#include "SPIC.h"

void SPI_InitPIO(void)
{
	//Because we are using PORTB.PIN25 in peripheral B mode
	//	we need to enable the clock for that line.
	//PMC->PMC_PCER0 |= _BV(ID_PIOA);
	
	//configure for input
	PIOA->PIO_PDR |= PIO_PA25;
	PIOA->PIO_ODR |= PIO_PA25;		//Input
	PIOA->PIO_PUER |= PIO_PA25;		//pull-up
	
	PIOA->PIO_PDR |= PIO_PA26;		//MOSI
	PIOA->PIO_OER |= PIO_PA26;		//MOSI	Output
	PIOA->PIO_ABSR &= ~PIO_PA26;	//Peripheral A
	
	PIOA->PIO_PDR |= PIO_PA27;		//SPCK
	PIOA->PIO_OER |= PIO_PA27;		//SPCK	Output
	PIOA->PIO_ABSR &= ~PIO_PA27;	//Peripheral A
	
	PIOA->PIO_PDR |= PIO_PA28;		//NPCS0
	PIOA->PIO_OER |= PIO_PA28;		//NPCS0	Output
	PIOA->PIO_ABSR &= ~PIO_PA28;	//Peripheral A
	PIOA->PIO_PUER |= PIO_PA28;		//pull-up
}

void SPI_Init(void)
{
	int i;
	pmc_enable_periph_clk(ID_SPI0);
	spi_set_writeprotect(SPI0, 0);
	SPI_InitPIO();
	spi_reset(SPI0);	
	for(i=0;i<0xffff;i++)
		__NOP();
	spi_disable(SPI0);
	spi_set_master_mode(SPI0);
	spi_disable_mode_fault_detect(SPI0);
	spi_disable_loopback(SPI0);
	spi_set_peripheral_chip_select_value(SPI0, spi_get_pcs(DEFAULT_CHIP_ID)); //NPCS[3:0]=1110b
	spi_set_variable_peripheral_select(SPI0);
	spi_disable_peripheral_select_decode(SPI0);
	spi_set_delay_between_chip_select(SPI0, 6);
	spi_set_clock_polarity(SPI0,0,false);
	spi_set_clock_phase(SPI0, 0, true);
	spi_set_baudrate_div(SPI0,0,21); //SCLK = 4Mhz
	spi_set_bits_per_transfer(SPI0,0,0);
	spi_set_transfer_delay(SPI0,0,10,1); //
	spi_configure_cs_behavior(SPI0, 0, SPI_CS_KEEP_LOW);
	SPI0->SPI_IER = 0;	
	spi_enable(SPI0);
	spi_set_writeprotect(SPI0, 1);
}

/*
**
* \brief Write command to RF module with SR reading
*
* \param data Pointer to the command vector.
* \param length The length of the command vector.
* \param SR Pointer to the status register need to be read from the RF module.
*
* \retval 1 on Success.
* \retval 0 on Failure.
*/
uint16_t SPIC_Write(uint16_t *data, int length, uint16_t *SR)
{
	int i;
	uint8_t Pcs;
	if(SPI0->SPI_SR & SPI_SR_RDRF) // Something to clear
		spi_read(SPI0,SR,&Pcs);	
	if(length>1)
	{
		for (i=0; i<(length-1);i++)
		{		
			if(spi_write(SPI0,data[i],0,0) != SPI_OK)
				return 0;
			if(i==0)
				spi_read(SPI0,SR,&Pcs);
		}
		if(spi_write(SPI0,data[length-1],0,1) != SPI_OK)
			return 0;
	}
	else
	{		
		if(spi_write(SPI0,data[0],0,1) != SPI_OK)
			return 0;
		spi_read(SPI0,SR,&Pcs);
	}
	return 1;
}

/*
**
* \brief Read command to RF module with SR reading
*
* \param data Pointer to the command vector.
* \param length The length of the command vector.
* \param SR Pointer to the status register need to be read from the RF module.
*
* \retval 1 on Success.
* \retval 0 on Failure.
*/
uint16_t SPIC_Read(uint16_t *data, int length, uint16_t *SR, uint16_t *Resp)
{
	int i;
	uint8_t Pcs;
	if(SPI0->SPI_SR & SPI_SR_RDRF) // Something to clear
		spi_read(SPI0,SR,&Pcs);
	for (i=0; i<(length-1);i++)
	{
		if(spi_write(SPI0,data[i],0,0) != SPI_OK)	
			return 0;
		if(i==0)
			spi_read(SPI0,SR,&Pcs);
		else
			spi_read(SPI0,&Resp[i-1],&Pcs);
	}
	if(spi_write(SPI0,data[length-1],0,1) != SPI_OK)	
		return 0;
	spi_read(SPI0,&Resp[length-2],&Pcs);
	return 1;
}