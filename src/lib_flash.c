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
******************************************************************************/
#include "lib_flash.h"
#include "ch32fun.h"

#include <stddef.h>
#include <string.h>


flash_lock_state_t flash_get_lock_state(void)
{
	// If the LOCK bit is set, the flash is fully locked
	if(FLASH->CTLR & FLASH_CTLR_LOCK)   return FLASH_LS_LOCKED;

	// If the FLOCK bit is set, the Flash is only SLOW mode unlocked
	if(FLASH->CTLR & FLASH_CTLR_FLOCK)  return FLASH_LS_UNLOCKED_NORM;

	// If neither flash is set, the flash is completely unlocked
	return FLASH_LS_UNLOCKED_FAST;
}


flash_err_t flash_unlock(void)
{
	flash_err_t ret = FLASH_OK;
	// If the flash is already unlocked, return OK
	if(flash_get_lock_state() != FLASH_LS_UNLOCKED_FAST)
	{
		// Normal Mode Unlock
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
		// Wait for the Flash to finish
		while(FLASH->STATR & FLASH_STATR_BSY);

		// FAST Mode Unlock
		FLASH->MODEKEYR = FLASH_KEY1;
		FLASH->MODEKEYR = FLASH_KEY2;
		// Wait for the Flash to finish
		while(FLASH->STATR & FLASH_STATR_BSY);

		// If the flash is NOT in UNLOCKED_FAST mode, return an error
		if(flash_get_lock_state() != FLASH_LS_UNLOCKED_FAST)
			ret = FLASH_ERR_LOCKED;
	}
	return ret;
}


flash_err_t flash_lock(void)
{
	FLASH->CTLR |= FLASH_CTLR_LOCK;

	if(flash_get_lock_state() != FLASH_LS_LOCKED)
		return FLASH_ERR_UNLOCKED;

	return FLASH_OK;
}


flash_err_t flash_read_page(const size_t page_num, flash_page_t *page_ptr)
{
	if(page_ptr == NULL)
		return FLASH_ERR_INVALID_PTR;

	if(page_num > FLASH_MAX_PAGE)
		return FLASH_ERR_PAGE_OUT_OF_RANGE;

	// Calculate the flash and data pointers
	volatile const uint32_t *flsh_ptr = (volatile const uint32_t*)(FLASH_BASE + (page_num * FLASH_PAGE_BYTES));
	uint32_t *data_ptr = (uint32_t*)page_ptr->byte;

	for(uint8_t w = 0; w < FLASH_PAGE_WORDS; w++)
		*data_ptr++ = *flsh_ptr++;

	return FLASH_OK;
}


flash_err_t flash_write_page(const size_t page_num, const flash_page_t *page_ptr)
{
	if(page_ptr == NULL)
		return FLASH_ERR_INVALID_PTR;

	if(page_num > FLASH_MAX_PAGE)
		return FLASH_ERR_PAGE_OUT_OF_RANGE;

	// Clear the buffer
	FLASH->CTLR |= CR_BUF_RST | CR_PAGE_PG;
	while(FLASH->STATR & FLASH_STATR_BSY);

	// Calculate the data and flash pointers, then set the flash addr
	volatile uint32_t *flsh_ptr = (volatile uint32_t*)(FLASH_BASE + (page_num * FLASH_PAGE_BYTES));
	const uint32_t *data_ptr = (const uint32_t*)page_ptr->byte;
	FLASH->ADDR = FLASH_BASE + (page_num * FLASH_PAGE_BYTES);

	// Copy over 16 words, set BUF_LOAD and wait
	for(uint8_t w = 0; w < FLASH_PAGE_WORDS; w++)
	{
		*flsh_ptr++ = *data_ptr++;
		FLASH->CTLR |= FLASH_CTLR_BUF_LOAD | CR_PAGE_PG;
		while(FLASH->STATR & FLASH_STATR_BSY);
	}

	// Start the final write-out and wait
	FLASH->CTLR |= CR_STRT_Set | CR_PAGE_PG;
	while(FLASH->STATR & FLASH_STATR_BSY);

	return FLASH_OK;
}


flash_err_t flash_erase_page(const size_t page_num)
{
	if(page_num > FLASH_MAX_PAGE)
		return FLASH_ERR_PAGE_OUT_OF_RANGE;

	// Signal an Erase
	FLASH->CTLR = CR_PAGE_ER;

	// Set the flash Address
	FLASH->ADDR = FLASH_BASE + (page_num * FLASH_PAGE_BYTES);

	// Erase and wait
	FLASH->CTLR |= CR_STRT_Set | CR_PAGE_ER;
	while(FLASH->STATR & FLASH_STATR_BSY);

	// Confirm the Erase has worked
	const uint32_t *flsh_ptr = (const uint32_t*)(FLASH_BASE + (page_num * FLASH_PAGE_BYTES));
	for(uint8_t w = 0; w < FLASH_PAGE_WORDS; w++)
		if(*flsh_ptr++ != 0xFFFFFFFF) return FLASH_ERR_ERASE_FAILED;

	return FLASH_OK;
}
