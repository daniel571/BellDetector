/*
 * RF24.h
 *
 * Created: 13/04/2019 15:54:08
 *  Author: Daniel
 */ 


#ifndef RF24_H_
#define RF24_H_

#include <stdint.h>
#include <stdbool.h>

#define RF24_MAX_LEN_DATA 5
#define _BV(x) (1<<(x))
/**
 * Power Amplifier level.
 *
 * For use with setPALevel()
 */
typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;

/**
 * Data rate.  How fast data moves through the air.
 *
 * For use with setDataRate()
 */
typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;

/**
 * CRC Length.  How big (if any) of a CRC is included.
 *
 * For use with setCRCLength()
 */
typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

typedef enum {RF24_TEST=0, RF24_BELLDOOR} rf24_datatypes;

/**
 * Driver for nRF24L01(+) 2.4GHz Wireless Transceiver
 */

struct RF24
{
  uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
  uint8_t csn_pin; /**< SPI Chip select */
  bool wide_band; /* 2Mbs data rate in use? */
  bool p_variant; /* False for RF24L01 and true for RF24L01P */
  uint8_t payload_size; /**< Fixed size of payloads */
  bool ack_payload_available; /**< Whether there is an ack payload waiting */
  bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */ 
  uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
  uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */
};

void RF_InitTX(void);
void RF_OpenWritePipe(void);
void RF_SendBell(uint16_t CmdID);

void RF_ActiveUp(void);
void RF_DisActive(void);
void RF_PowerUpTXmode(void);
void RF_PowerDown(void);
void RF_SetPALevel(rf24_pa_dbm_e level);
void RF_setChannel(uint8_t channel);
bool RF_setDataRate(rf24_datarate_e speed);
void RF_setCRCLength(rf24_crclength_e length);
void writeBuf_register(uint8_t reg, const uint16_t* Buf, int Len);
void writeByte_register(uint8_t reg, uint8_t value);
uint8_t readByte_register(uint8_t reg);
void readBuf_register(uint8_t reg, uint16_t* Resp);
void flush_tx(void);
void flush_rx(void);

  /**@}*/

#endif /* RF24_H_ */