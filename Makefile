###############################################################################
# CH32V003 Specific Makefile

### Project Specific Variables ################################################
SRC_DIR    := ./src
BUILD_DIR  := ./build
TOOLKIT    := ./toolkit

# Project name
TARGET := flash_nvs_demo
# MCU Name - Only CH32V00x Supported
TARGET_MCU ?= CH32V003

# Compiles every additional .c file
TARGET_SOURCE_FILES := $(filter-out $(SRC_DIR)/$(TARGET).c, $(wildcard $(SRC_DIR)/*.c))

### System Variables ##########################################################
# Cross-compiler prefix
PREFIX := riscv64-unknown-elf

# Set the minichlink executable
MINICHLINK := $(TOOLKIT)/minichlink

# Set newlib dir. Fedora places newlib in a different location
ifneq ($(wildcard /etc/fedora-release),)
	NEWLIB?=/usr/arm-none-eabi/include
else
	NEWLIB?=/usr/include/newlib
endif


WRITE_SECTION?=flash
SYSTEM_C := $(TOOLKIT)/ch32fun.c

# Debug flag
ifeq ($(DEBUG),1)
	EXTRA_CFLAGS+=-DFUNCONF_DEBUG=1
endif

# Set the initial C Compiler Flags, and the linker flags
CFLAGS := \
	-g -Os -flto -ffunction-sections -fdata-sections -fmessage-length=0 \
	-msmall-data-limit=8
	
LDFLAGS := -Wl,--print-memory-usage -Wl,-Map=$(BUILD_DIR)/$(TARGET).map -lgcc


ifeq ($(findstring CH32V00,$(TARGET_MCU)),CH32V00) # CH32V002, 3, 4, 5, 6, 7
	MCU_PACKAGE?=1
	
	#GENERATED_LD_FILE?=$(CH32FUN)/generated_ch32v003.ld
	#LINKER_SCRIPT?=$(GENERATED_LD_FILE)
	#LDFLAGS+=-L$(CH32FUN)/../misc -lgcc
	
	
	# Set the Architecture Flags. If not GCC 13 or higher, does not support zmmul
	GCCVERSION13 := $(shell expr `$(PREFIX)-gcc -dumpversion | cut -f1 -d.` \>= 13)
	ifeq ($(TARGET_MCU),CH32V003)
		CFLAGS_ARCH+=-march=rv32ec -mabi=ilp32e
	else ifeq "$(GCCVERSION13)" "1"
		CFLAGS_ARCH+=-march=rv32ec_zmmul -mabi=ilp32e -DCH32V00x=1
	else
		CFLAGS_ARCH+=-march=rv32ec -mabi=ilp32e -DCH32V00x=1
	endif


	CFLAGS_ARCH+=-D$(TARGET_MCU)

	ifeq ($(findstring CH32V003, $(TARGET_MCU)), CH32V003)
		TARGET_MCU_LD:=0
	else ifeq ($(findstring CH32V002, $(TARGET_MCU)), CH32V002)
		TARGET_MCU_LD:=5
	else ifeq ($(findstring CH32V004, $(TARGET_MCU)), CH32V004)
		TARGET_MCU_LD:=6
	else ifeq ($(findstring CH32V005, $(TARGET_MCU)), CH32V005)
		TARGET_MCU_LD:=7
	else ifeq ($(findstring CH32V006, $(TARGET_MCU)), CH32V006)
		TARGET_MCU_LD:=7
	else ifeq ($(findstring CH32V007, $(TARGET_MCU)), CH32V007)
		TARGET_MCU_LD:=7
	else
		ERROR:=$(error Unknown MCU $(TARGET_MCU))
	endif
else
	ERROR:=$(error Uknown or Unsupported MCU $(TARGET_MCU)... (CH32V00x ONLY))
endif

# Create the LD File based on the selected MCU
GENERATED_LD_FILE := $(BUILD_DIR)/generated_$(TARGET_MCU_PACKAGE).ld
LINKER_SCRIPT?=$(GENERATED_LD_FILE)


# Compiler flags, warnings, dirs etc. Add more directories here as needed
CFLAGS += \
	$(CFLAGS_ARCH) -static-libgcc \
	-I$(NEWLIB) \
	-I$(TOOLKIT) \
	-I$(SRC_DIR) \
	-nostdlib \
	-Wall $(EXTRA_CFLAGS)

# Add to the linker flags
LDFLAGS += -T $(LINKER_SCRIPT) -Wl,--gc-sections -L$(TOOLKIT)

# All .c files to compile, just adds ch32v003_fun, target and extra files
FILES_TO_COMPILE := $(SYSTEM_C) $(SRC_DIR)/$(TARGET).c $(TARGET_SOURCE_FILES)


# TODO:
# Architecutre Compile Flags. Change these if using a different Chip 
#CFLAGS_ARCH += -march=rv32ec -mabi=ilp32e -DCH32V003=1


### Makefile dependencies #####################################################
### Default Goal ###
# Create the binary file and hex from the .elf file
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(PREFIX)-objdump -S $^ > $(BUILD_DIR)/$(TARGET).lst
	$(PREFIX)-objcopy -O binary $< $(BUILD_DIR)/$(TARGET).bin
	$(PREFIX)-objcopy -O ihex $< $(BUILD_DIR)/$(TARGET).hex


# Compile the .elf file - requires the compiled ld file, .c files and other depends
$(BUILD_DIR)/$(TARGET).elf: $(FILES_TO_COMPILE) $(LINKER_SCRIPT) $(EXTRA_ELF_DEPENDENCIES) | $(BUILD_DIR)
	$(PREFIX)-gcc -o $@ $(FILES_TO_COMPILE) $(CFLAGS) $(LDFLAGS)


# Generate the Linker Script
.PHONY : $(GENERATED_LD_FILE)
$(GENERATED_LD_FILE) : $(BUILD_DIR)
	$(PREFIX)-gcc -E -P -x c -DTARGET_MCU=$(TARGET_MCU) -DMCU_PACKAGE=$(MCU_PACKAGE) -DTARGET_MCU_LD=$(TARGET_MCU_LD) $(TOOLKIT)/ch32fun.ld > $(GENERATED_LD_FILE)


# Ensure the Build Directory is created
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


# Independently build ch32fun.o, instead of recompiling from source every time.
ch32fun.o : $(SYSTEM_C)
	$(PREFIX)-gcc -c -o $@ $(SYSTEM_C) $(CFLAGS)


closechlink :
	-killall minichlink
	
terminal : monitor

monitor :
	$(TOOLKIT)/minichlink -T

unbrick :
	$(TOOLKIT)/minichlink -u

gdbserver : 
	-$(TOOLKIT)/minichlink -baG

gdbclient :
	gdb-multiarch $(TARGET).elf -ex "target remote :3333"

clangd :
	make clean
	bear -- make build

clangd_clean :
	rm -f compile_commands.json
	rm -rf .cache

clean :
	rm -r $(BUILD_DIR)

flash: $(BUILD_DIR)/$(TARGET).bin
	#$(MINICHLINK) -D               # Disable NRST, use ad GPIO
	$(MINICHLINK) -w $< flash -b




build: $(BUILD_DIR)/$(TARGET).bin
