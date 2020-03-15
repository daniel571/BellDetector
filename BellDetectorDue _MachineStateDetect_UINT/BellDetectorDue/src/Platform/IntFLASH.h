/*
 * IntFLASH.h
 *
 * Created: 08/06/2019 20:23:29
 *  Author: Daniel
 */ 


#ifndef INTFLASH_H_
#define INTFLASH_H_
#include <asf.h>
#include <string.h>

/* Internal flash page size. */
# define IFLASH_PAGE_SIZE     IFLASH1_PAGE_SIZE
/* Internal flash lock region size. */
# define IFLASH_LOCK_REGION_SIZE     IFLASH1_LOCK_REGION_SIZE
// 1Kb of data
#define DATA_LENGTH   ((IFLASH1_PAGE_SIZE/sizeof(byte))*4)

// choose a start address that's offset to show that it doesn't have to be on a page boundary
#define  FLASH_START  ((Byte *)IFLASH1_ADDR)

/*! \name Flash driver return codes */
//! @{
typedef enum flash_rc {
	FLASH_RC_OK = 0,        //!< Operation OK
	FLASH_RC_YES = 0,       //!< Yes
	FLASH_RC_NO = 1,        //!< No
	FLASH_RC_ERROR = 0x10,  //!< General error
	FLASH_RC_INVALID,       //!< Invalid argument input
	FLASH_RC_NOT_SUPPORT = 0xFFFFFFFF    //!< Operation is not supported
} flash_rc_t;

uint32_t flash_write(uint32_t ul_address, const void *p_buffer,uint32_t ul_size, uint32_t ul_erase_flag);
uint32_t FLASH_Init(void);
Byte* ReadCalibFromFlash(void);
void WriteCalibToFlash(Byte* Calib);
uint32_t flash_unlock(uint32_t ul_start, uint32_t ul_end,uint32_t *pul_actual_start, uint32_t *pul_actual_end);
uint32_t flash_lock(uint32_t ul_start, uint32_t ul_end,uint32_t *pul_actual_start, uint32_t *pul_actual_end);


#endif /* INTFLASH_H_ */