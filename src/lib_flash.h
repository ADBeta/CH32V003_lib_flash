/******************************************************************************
* CH32V003 lib_flash - simple and lightweight library to support:
* * FAST_MODE Read
* * FAST_MODE Write
* * Erase
* * Lock
* * Unlock
*
* See GitHub Repo for more information: 
* https://github.com/ADBeta/CH32V003_lib_flash
*
* Released under the MIT Licence
* Copyright ADBeta (c) 2025
*
* Ver 1.0.0    11 Jul 2025
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
#define FLASH_PAGE_BYTES 64    // Bytes Per Page
#define FLASH_PAGE_WORDS 16    // Words Per Page


/*** Structs & Types *********************************************************/
typedef enum {
	FLASH_OK                        = 0x00,
	FLASH_ERR_LOCKED,
	FLASH_ERR_UNLOCKED,
	FLASH_ERR_PAGE_OUT_OF_RANGE,
	FLASH_ERR_INVALID_PTR,

	FLASH_ERR_ERASE_FAILED,

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


/// @brief Locks the flash
/// @param None
/// @return flash_err_t error status, FLASH_OK if successful
flash_err_t flash_lock(void);


/// @brief Reads a page of flash to the passed pointer.
/// @param page_num, Page to read   (0->256)
/// @param page_ptr, buffer the page will be read into
/// @return flash_err_t error status, FLASH_OK if successfull
flash_err_t flash_read_page(const size_t page_num, flash_page_t *page_ptr);


/// @brief Writes page data to a page of flash
/// @param page_num, Page to write to (0->256)
/// @param page_ptr, Data to write
/// @return flash_err_t error status, FLASH_OK if successful
flash_err_t flash_write_page(const size_t page_num, const flash_page_t *page_ptr);


/// @brief Erases a Page of flash
/// @param page_num, Page to Erase (0->256)
/// @return flash_err_t error status, FLASH_OK if successful
flash_err_t flash_erase_page(const size_t page_num);


#endif
