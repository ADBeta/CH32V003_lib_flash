/******************************************************************************
* Basic Example of using lib_nvs on the CH32V003 Microcontroller
* 
* See GitHub Repo for more information: 
* https://github.com/ADBeta/CH32V003_lib_nvs
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

	// Unlock the Flash
	if(flash_unlock() != FLASH_OK) 
	{
		printf("Error: Flash Unlock Failed.\n"); 
		return -1;
	}


	//Erase the flash page first
	flash_erase_page(255);

	// Create a flash page, then read in data from the 255th page
	flash_page_t page = {0};
	flash_read_page(255, &page);

	// Print the page data
	printf("\n\nBytes @ Page 255:\n");
	flash_print_page(&page);



	while(1)
	{

	}
}
