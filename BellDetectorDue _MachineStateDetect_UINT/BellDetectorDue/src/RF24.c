/*
 * RF24.c
 *
 * Created: 13/04/2019 15:53:51
 *  Author: Daniel
 */ 
#include "nRF24L01.h"
#include "RF24.h"
#include "SPIC.h"
#include "Prints.h"
#include "FIFO.h"
#include "Timer.h"
#define DEBUG_LOG 1

static uint16_t StatusRegister;
const uint16_t pipe0[5] = {0xF0,0xF0,0xF0,0xF0,0xF1};

// When writing 0x1234 to SPI only 0x34 will be written

// In multi-byte registers read or write to the MSBit of the LSByte first.
void writeByte_register(uint8_t reg, uint8_t value)
{
	//uint8_t Test;
	//char out_str2[30] = {0};
	uint16_t Cmd[2];
	Cmd[0] = W_REGISTER | (REGISTER_MASK & reg);
	Cmd[1] = value;
	if(!SPIC_Write(Cmd, 2, &StatusRegister))
		PrintLn("Error\n");
	//Test = readByte_register(reg);
	/*#if DEBUG_LOG
	if(Test == value )
	{
		char out_str2[30] = {0};
		sprintf(out_str2,"Great reg %u!! \n", reg);
		PrintLn(out_str2);
	}
	else
	{
		char out_str2[30] = {0};
		sprintf(out_str2,"we have %u we set %u on reg %u\n", Test,value,reg);
		PrintLn(out_str2);
	}
	#endif*/
	
	//sprintf(out_str2,"Reg %u is %u write data %u\n", reg,Test,value);
	//PrintLn(out_str2);
}

void writeBuf_register(uint8_t reg, const uint16_t* Buf, int Len)
{
	int i;
	uint16_t Cmd[RF24_MAX_LEN_DATA+1];
	
	Cmd[0] = W_REGISTER | (REGISTER_MASK & reg);
	for(i=0;i<Len;i++)
		Cmd[i+1] = Buf[i];	
	
	if(!SPIC_Write(Cmd, Len+1, &StatusRegister))
		PrintLn("Error2\n");
	
}

uint8_t readByte_register(uint8_t reg)
{
	uint16_t Resp;
	uint16_t Cmd[2];
	char out_str[30] = {0};
		
	Cmd[0] = R_REGISTER| (REGISTER_MASK & reg);
	Cmd[1] = 0;
	SPIC_Read(Cmd, 2, &StatusRegister,&Resp);
	
	sprintf(out_str,"read reg %u val %u \n", reg,Resp);
	//PrintLn(out_str);
	return (uint8_t)(Resp);
}

void readBuf_register(uint8_t reg, uint16_t* Resp)
{
	int i;
	uint16_t Cmd[6];
	char out_str[30] = {0};
	//uint16_t Cmd[RF24_MAX_LEN_DATA+1];
	
	Cmd[0] = R_REGISTER | (REGISTER_MASK & reg);
	for(i=0;i<5;i++)
		Cmd[i+1] = 0;
	
	SPIC_Read(Cmd, 6, &StatusRegister, Resp);
	sprintf(out_str,"read reg %u val %u %u %u %u %u\n", reg,Resp[0],Resp[1],Resp[2],Resp[3],Resp[4]);
	//PrintLn(out_str);
}

void RF_ActiveUp()
{
	pio_set_pin_high(LED1_GPIO);
}

void RF_DisActive()
{
	pio_set_pin_low(LED1_GPIO);
}

void RF_PowerUpTXmode()
{
	int i;
	writeByte_register(CONFIG,(readByte_register(CONFIG)) | ((1<<PWR_UP) & ~_BV(PRIM_RX)) );
	readByte_register(CONFIG);
	for(i=0;i<0xff;i++)
		__NOP;
}

void RF_SetPALevel(rf24_pa_dbm_e level)
{
	uint8_t setup = readByte_register(RF_SETUP) ;
	setup |= (1<<RF_PWR_LOW);
	setup &= ~(1<<RF_PWR_HIGH);

	// switch uses RAM (evil!)
	if ( level == RF24_PA_MAX )
	{
		setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
	}
	else if ( level == RF24_PA_HIGH )
	{
		setup |= _BV(RF_PWR_HIGH) ;
	}
	else if ( level == RF24_PA_LOW )
	{
		setup |= _BV(RF_PWR_LOW);
	}
	else if ( level == RF24_PA_MIN )
	{
		// nothing
	}
	else if ( level == RF24_PA_ERROR )
	{
		// On error, go to maximum PA
		setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)) ;
	}

	writeByte_register( RF_SETUP, setup ) ;
}


bool RF_setDataRate(rf24_datarate_e speed)
{
	bool result = false;
	uint8_t setup = readByte_register(RF_SETUP) ;

	// HIGH and LOW '00' is 1Mbs - our default
	//wide_band = false ;
	setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;
	if( speed == RF24_250KBPS )
	{
		// Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
		// Making it '10'.
	//	wide_band = false ;
		setup |= _BV( RF_DR_LOW ) ;
	}
	else
	{
		// Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
		// Making it '01'
		if ( speed == RF24_2MBPS )
		{
			//wide_band = true ;
			setup |= _BV(RF_DR_HIGH);
		}
		else
		{
			// 1Mbs
			//wide_band = false ;
		}
	}
	writeByte_register(RF_SETUP,setup);

	// Verify our result
	/*if ( (setup2=readByte_register(RF_SETUP)) == setup )
	{
		result = true;
	//	PrintLn("true\n");
	}
	#if DEBUG_LOG
	else
	{
		char out_str2[30] = {0};
	//	sprintf(out_str2,"Daniel %u setup %u \n", setup2,setup);
		//PrintLn(out_str2);
	}
	#endif*/
	return result;
}

void RF_setCRCLength(rf24_crclength_e length)
{
	uint8_t config = readByte_register(CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;
	
	// switch uses RAM (evil!)
	if ( length == RF24_CRC_DISABLED )
	{
		// Do nothing, we turned it off above.
	}
	else if ( length == RF24_CRC_8 )
	{
		config |= _BV(EN_CRC);
	}
	else
	{
		config |= _BV(EN_CRC);
		config |= _BV( CRCO );
	}
	writeByte_register( CONFIG, config ) ;
}


void RF_setChannel(uint8_t channel)
{
	//const uint8_t max_channel = 127;
	uint8_t RF_CH2;
	
	writeByte_register(RF_CH,channel);
	// Verify our result
	if ( (RF_CH2=readByte_register(RF_CH)) == channel )
	{
	//	PrintLn("GREAT!!\n");
	}
	else
	{
		//char out_str2[30] = {0};
		//sprintf(out_str2,"we have %u we set %u \n", RF_CH2,channel);
	//	PrintLn(out_str2);
	}
	
}
void flush_rx(void)
{
	uint16_t Cmd[1];
	Cmd[0] = FLUSH_RX;
	if(!SPIC_Write(Cmd, 1, &StatusRegister))
		PrintLn("Error\n");
}

/****************************************************************************/

void flush_tx(void)
{
	uint16_t Cmd[1];
	Cmd[0] = FLUSH_TX;
	if(!SPIC_Write(Cmd, 1, &StatusRegister))
		PrintLn("Error\n");
}

void RF_OpenWritePipe()
{
	writeBuf_register(TX_ADDR, pipe0, 5);
}

void RF_InitTX()
{
	uint16_t Response[5]= {0,0,0,0,0};
	int i;
	RF_DisActive();
	//little delay
	for(i=0;i<0xff;i++)
		__NOP;
	
	
	// Disable auto-retransmit
	writeByte_register(SETUP_RETR,(0b0100 << ARD) | (0 << ARC));
	// You can configure the RF24 when power-down , stand-by or RX,TX mode
	RF_SetPALevel(RF24_PA_MAX);
	if(!RF_setDataRate(RF24_250KBPS))
		PrintLn("Error3\n");
	// Initialize no CRC
	RF_setCRCLength(RF24_CRC_DISABLED);
	// Disable dynamic payloads, to match dynamic_payloads_enabled setting
	writeByte_register(DYNPD,0);
	// Reset current status
	// Notice reset and flush is the last thing we do
	writeByte_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
	// Set up default configuration.  Callers can always change it later.
	// This channel should be universally safe and not bleed over into adjacent
	// spectrum.
	RF_setChannel(76);
	// NO_ACK Mode
	writeByte_register(EN_AA, 0);
	// Flush buffers
	flush_rx();
	flush_tx();
	// Route the packet to PTX address at pipe0
	RF_OpenWritePipe();
	RF_PowerUpTXmode();
	
	RF_ActiveUp();
	/*readByte_register(RF_SETUP);
	readByte_register(CONFIG);
	readByte_register(RF_CH);
	readByte_register(SETUP_RETR);
	readByte_register(DYNPD);
	readByte_register(STATUS);
	readByte_register(EN_AA);*/
	readBuf_register(TX_ADDR,Response);
	
}


void RF_SendBell(uint16_t CmdID)
{
	uint16_t Cmd[2];
	writeByte_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
	Cmd[0] = W_TX_PAYLOAD;
	Cmd[1] = CmdID;
	Timer_Sleep(10);
	if(!SPIC_Write(Cmd, 2, &StatusRegister))
		PrintLn("Error\n");
	Timer_Sleep(10);
	PrintLn("Send RF \n");
}