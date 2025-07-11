/******************************************************************************
* Basic Example of using lib_nvs on the CH32V003 Microcontroller
* 
* See GitHub Repo for more information: 
* https://github.com/ADBeta/CH32V003_lib_flash
*
* Released under the MIT Licence
* Copyright ADBeta (c) 2025
******************************************************************************/
#include "ch32fun.h"
#include "lib_flash.h"

#include <stdio.h>


/*** Functions ***************************************************************/
void flash_print_page(const flash_page_t *page_ptr)
{
	// Print each byte as an upper case hex value, with 16 bytes per line
	size_t row = 0;
	for(size_t b = 0; b < sizeof(page_ptr->byte); b++)
	{
		printf("%02X  ", page_ptr->byte[b]);
		if(row++ == 15) { printf("\n"); row = 0; }
	}
}


/*** Main ********************************************************************/
int main() 
{
	SystemInit();

	printf("\n\n--------------- CH32V003 lib_flash Demo ---------------\n\n");

	// Unlock the Flash
	printf("Unlocking flash.....");
	if(flash_unlock() != FLASH_OK) { printf("Failed.\n"); return -1; }
	printf("Done\n");


	// Erase the flash page first
	// NOTE: Comment this chunk in order to confirm flash changes persist
	printf("Erasing Flash Page 255....");
	if(flash_erase_page(255) != FLASH_OK) {printf("Failed\n"); return -1; }
	printf("Done\n");


	// Create a flash page, then read in data from the 255th page
	flash_page_t page = {0};
	flash_read_page(255, &page);
	printf("\n\nRead Flash Page 255...\n");
	flash_print_page(&page);


	// Change the values in the page data buffer for demo
	for(uint8_t x = 0; x < 64; x++)
		page.byte[x] = x;


	// Write the modified page data out, then print it.
	flash_write_page(255, &page);
	printf("\n\nOverwritten Flash Page 255...\n");
	flash_print_page(&page);


	// Do Nothing.i
	while(1)
	{

	}

	return 0;
}
