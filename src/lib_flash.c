/******************************************************************************
*
*
******************************************************************************/
#include "lib_flash.h"
#include "ch32fun.h"
#include <stddef.h>



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










flash_err_t flash_read_page(const size_t page_num, flash_page_t *page_ptr)
{
	if(page_ptr == NULL)
		return FLASH_ERR_INVALID_PTR;

	if(page_num > FLASH_MAX_PAGE)
		return FLASH_ERR_PAGE_OUT_OF_RANGE;

	// Calculate the starting flash pointer
	const uint8_t *fp = (const uint8_t *)(FLASH_BASE + (page_num * 64));

	// Loop though the 64 bytes in flash
	for(uint8_t b_idx = 0; b_idx < 64; b_idx++)
		page_ptr->byte[b_idx] = fp[b_idx];

	return FLASH_OK;
}
