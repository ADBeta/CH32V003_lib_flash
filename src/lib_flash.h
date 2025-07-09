/******************************************************************************
*
*
*
*
* NOTE: This library only supports FAST Mode, but abstractions have been done
* to allow NORM/SLOW mode to be added by some brave soul at a later date
******************************************************************************/
#ifndef CH32_LIB_FLASH_H
#define CH32_LIB_FLASH_H

#include "ch32fun.h"
#include <stddef.h>

/*** Definitions *************************************************************/
// Base Address of the Flash
#define FLASH_MAX_PAGE 256


/*** Structs & Types *********************************************************/
typedef enum {
	FLASH_OK                        = 0x00,
	FLASH_ERR_LOCKED,
	FLASH_ERR_PAGE_OUT_OF_RANGE,
	FLASH_ERR_INVALID_PTR,

} flash_err_t;

typedef enum {
	FLASH_LS_LOCKED         = 0x00,  // Flash is completely Locked
	FLASH_LS_UNLOCKED_NORM,          // NORMAL Mode is Unlocked (CTLR->LOCK)
	FLASH_LS_UNLOCKED_FAST,          // FAST mode is unlocked (CTLR->FLOCK)
} flash_lock_state_t;


// Flash Page of 64 Bytes (16 4-byte words)
typedef struct { 
	uint8_t byte[64];
} flash_page_t;

/*** Functions ***************************************************************/
/// @brief Returns the Locked State of the Flash. 
/// @param None
/// @return flash_lock_state_t flash lock state
flash_lock_state_t flash_get_lock_state(void);


/// @brief Unlocks the flash into FAST mode
/// @param None
/// @return flash_err_t error status. FLASH_OK if successful
flash_err_t flash_unlock(void);


// TODO: Lock feature


/// @brief Reads a page of flash to the passed pointer.
/// @param page_num, Page to read   (0->256)
/// @param page_ptr, buffer the page will be read into
/// @return flash_err_t error status, FLASH_OK if successfull
flash_err_t flash_read_page(const size_t page_num, flash_page_t *page_ptr);












#endif
