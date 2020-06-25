
PROJECT_NAME := project

CPU := nrf52840
CPUDEFINE := NRF52840_XXAA
LINKER_SCRIPT := nrf52840_xxaa.ld
HEAPSIZE := 8192
STACKSIZE := 8192

BINARY_NAME := $(PROJECT_NAME)_$(CPU)

# sources-files should contain:
#   * startup code (nrfx)
#   * system code (nrfx)
#   * required drivers (nrfx)
#   * your code
SOURCES_S := \
	nrfx/mdk/gcc_startup_$(CPU).S

SOURCES_C := \
	nrfx/mdk/system_$(CPU).c \
	nrfx/drivers/src/nrfx_uarte.c \
	nrfx/drivers/src/prs/nrfx_prs.c \
	syscalls.c \
	$(PROJECT_NAME).c

SOURCES_CXX := \

###

SDK_CONFIG_FILE := nrfx_config.h

BMP_DEVICE ?= /dev/ttyACM0

###

# if you want to explicitly see what commands are executed for each step,
# comment this:
.SILENT:

CROSSCOMPILE ?= arm-none-eabi-

CC := $(CROSSCOMPILE)gcc
CPP := $(CROSSCOMPILE)gcc
CXX := $(CROSSCOMPILE)g++
AS := $(CROSSCOMPILE)as
AR := $(CROSSCOMPILE)ar
LD := $(CROSSCOMPILE)g++
GDB := $(shell which $(CROSSCOMPILE)gdb || which gdb-multiarch || echo false)
NM := $(CROSSCOMPILE)nm
OBJCOPY := $(CROSSCOMPILE)objcopy
OBJDUMP := $(CROSSCOMPILE)objdump
READELF := $(CROSSCOMPILE)readelf
SIZE := $(CROSSCOMPILE)size

###

SOURCES := $(SOURCES_S) $(SOURCES_C) $(SOURCES_CXX)

LISTINGS := $(addsuffix .lst,$(SOURCES))
STACKUSAGES := $(patsubst %.c,%.su,$(SOURCES_C)) $(patsubst %.cpp,%.su,$(SOURCES_CXX))

OBJECTS_S := $(patsubst %.s,%.o,$(patsubst %.S,%.o,$(SOURCES_S)))
OBJECTS_C := $(patsubst %.c,%.o,$(SOURCES_C))
OBJECTS_CXX := $(patsubst %.cpp,%.o,$(SOURCES_CXX))
OBJECTS := $(OBJECTS_S) $(OBJECTS_C) $(OBJECTS_CXX)

###

MCU_FLAGS += -mcpu=cortex-m4 -mthumb -mabi=aapcs
MCU_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

DEFINE_FLAGS += -DCONFIG_GPIO_AS_PINRESET
DEFINE_FLAGS += -D$(CPUDEFINE)
DEFINE_FLAGS += -D__HEAP_SIZE=$(HEAPSIZE)
DEFINE_FLAGS += -D__STACK_SIZE=$(STACKSIZE)

CXXC_INCLUDE_FLAGS += -Inrfx
CXXC_INCLUDE_FLAGS += -Inrfx/drivers/include
CXXC_INCLUDE_FLAGS += -Inrfx/hal
CXXC_INCLUDE_FLAGS += -Inrfx/mdk
CXXC_INCLUDE_FLAGS += -Inrfx/soc
CXXC_INCLUDE_FLAGS += -ICMSIS_5/CMSIS/Core/Include/
CXXC_INCLUDE_FLAGS += -I.

DEBUG_OPTIMIZE_FLAGS += -O3 -g -gdwarf-4

# if you are using LTO,
DEBUG_OPTIMIZE_FLAGS += -flto -fdevirtualize-at-ltrans
CXXC_EXTRA_FLAGS += -fsanitize=address
# listings won't be worth anything, as LTO generated 'GIMPLE' instead of code. Also, ASAN does not work without LTO.
#CXXC_EXTRA_FLAGS += -Wa,-aghlms=$<.lst

CXXC_EXTRA_FLAGS += -nostartfiles -nodefaultlibs -nostdlib
CXXC_EXTRA_FLAGS += -fdata-sections -ffunction-sections
CXXC_EXTRA_FLAGS += -fstack-usage
CXXC_EXTRA_FLAGS += -Wall -Wextra -Wshadow --pedantic
C_LANG_FLAGS += -std=c11
CXX_LANG_FLAGS += -std=c++17

ASFLAGS += $(MCU_FLAGS) $(DEBUG_OPTIMIZE_FLAGS) $(DEFINE_FLAGS)
CFLAGS += $(MCU_FLAGS) $(DEBUG_OPTIMIZE_FLAGS) $(DEFINE_FLAGS) $(CXXC_INCLUDE_FLAGS) $(CXXC_EXTRA_FLAGS) $(C_LANG_FLAGS)
CXXFLAGS += $(MCU_FLAGS) $(DEBUG_OPTIMIZE_FLAGS) $(DEFINE_FLAGS) $(CXXC_INCLUDE_FLAGS) $(CXXC_EXTRA_FLAGS) $(CXX_LANG_FLAGS)
LDFLAGS += $(MCU_FLAGS) $(DEBUG_OPTIMIZE_FLAGS)
LDFLAGS += -static
LDFLAGS += -Wl,--start-group -lgcc -lm -lc -lg -lstdc++ -lnosys -Wl,--end-group
LDFLAGS += -Wl,--gc-sections -Wl,--cref,-Map=$(@:%.elf=%.map)
LDFLAGS += -Lnrfx/mdk -T$(LINKER_SCRIPT)
# if you want to have garbage-collected sections listed during linker stage:
#LDFLAGS += -Wl,--print-gc-sections
# if you don't want the warnings about sections removed:
LDFLAGS += -Wl,--no-print-gc-sections

###

$(BINARY_NAME).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@
	$(SIZE) -Ax $@


%.asm: %.elf
	$(OBJDUMP) -dgCxwsSh --show-raw-insn $< > $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

###

.PHONY: all clean nrfx_config flash-bmp

all: $(BINARY_NAME).elf $(BINARY_NAME).asm $(BINARY_NAME).hex $(BINARY_NAME).bin

clean:
	-rm $(OBJECTS) $(LISTINGS) $(STACKUSAGES)
	-rm $(BINARY_NAME).asm $(BINARY_NAME).bin $(BINARY_NAME).elf $(BINARY_NAME).hex $(BINARY_NAME).map

nrfx_config:
	java -jar support/cmsisconfig/CMSIS_Configuration_Wizard.jar $(SDK_CONFIG_FILE)

flash-bmp: $(BINARY_NAME).elf
	# assuming:
	#  * Black Magic Probe connected to $(BMP_DEVICE)
	#  * compatible Nordic MCU connected via SWD
	$(GDB) $(BINARY_NAME).elf \
		-ex 'set confirm off' \
		-ex 'target extended-remote $(BMP_DEVICE)' \
		-ex 'mon assert_srst scan' \
		-ex 'mon swdp_scan' \
		-ex 'attach 1' \
		-ex 'mon erase_mass' \
		-ex 'run' \
		-ex 'load' \
		-ex 'compare-sections' \
		-ex 'quit'
	$(GDB) $(BINARY_NAME).elf \
		-ex 'set confirm off' \
		-ex 'target extended-remote $(BMP_DEVICE)' \
		-ex 'mon assert_srst scan' \
		-ex 'mon swdp_scan' \
		-ex 'attach 1' \
		-ex 'kill' \
		-ex 'quit'

flash-jlink: $(BINARY_NAME).bin
	# assuming:
	#  * any type of Segger JLINK that is usable with an nRF52 CPU
	#    (e.g. the embedded jlink on the DK)
	#  * compatible Nordic MCU connected via SWD
	#  * installed JLink Software
	printf "erase\nloadfile $<\nr\nq\n" | JLinkExe -nogui 1 -autoconnect 1 -device $(CPUDEFINE) -if swd -speed 4000
