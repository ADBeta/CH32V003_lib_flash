# CH32V003 lib_flash

A Lightweight and simple driver library to support flash on the CH32x MCU
* FAST_MODE Read
* FAST_MODE Write
* Erase
* Lock
* Unlock

## Usage
See `lib_flash_demo.c` for a usage example, or `lib_flash.h` for documentation  
Simply add the `lib_flash.h` and `lib_flash.c` files to your project, 
`#include "lib_flash.h"`.

## Building
A stripped down SDK has been provided, and running `make`/`make flash monitor`
in the root director will build the project. **NOTE:** This is only for testing
and demoing the library. Do not use this SDK for release projects.  

This project is based on [CNLohr's ch32fun](https://github.com/cnlohr/ch32fun)

----
ADBeta (c) 2025
