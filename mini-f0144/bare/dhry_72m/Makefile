# dhry_72m - MM32F0144C6P (MINI-F0144), Dhrystone 2.1 benchmark at 72 MHz
# Toolchain: arm-none-eabi-gcc
# NOTE: no -flto! LTO would move benchmark code out of the timed loop.

CROSS      ?= arm-none-eabi-
CC          = $(CROSS)gcc
OBJCOPY     = $(CROSS)objcopy
SIZE        = $(CROSS)size

# Vendor library root (relative to this Makefile)
LIB         ?= ../../../LibSamples_MM32F0140_V1.9.11

DEV_DIR     = $(LIB)/Device/MM32F0140
CMSIS_DIR   = $(LIB)/Device/CMSIS/KEIL_Core
HAL_DIR     = $(DEV_DIR)/HAL_Lib/Src

# Application + HAL sources (no LTO used anywhere)
APP_SRCS    = main.c platform.c dhry_1.c dhry_2.c utils.c mm32f0140_it.c syscalls.c
HAL_SRCS    = hal_adc.c hal_comp.c hal_crc.c hal_dbg.c hal_div.c hal_dma.c \
              hal_exti.c hal_flash.c hal_flexcan.c hal_gpio.c hal_i2c.c \
              hal_iwdg.c hal_misc.c hal_pwr.c hal_rcc.c hal_spi.c hal_tim.c \
              hal_uart.c hal_uid.c hal_ver.c hal_wwdg.c

APP_OBJS    = $(addprefix build/,$(APP_SRCS:.c=.o))
HAL_OBJS    = $(addprefix build/,$(HAL_SRCS:.c=.o))
SYS_OBJS    = build/system_mm32f0140.o
OBJS        = $(APP_OBJS) $(HAL_OBJS) $(SYS_OBJS) build/startup_mm32f0140.o

BUILD_DIR   = build

INC         = -I. \
              -I$(DEV_DIR)/Include \
              -I$(DEV_DIR)/HAL_Lib/Inc \
              -I$(CMSIS_DIR)

DEFS        = -DUSE_STDPERIPH_DRIVER

# -Ofast -ffp-contract=fast -funroll-loops is the optimization used for the
# recorded result. Do NOT add -flto (would move code out of the timed loop).
CFLAGS      = -mcpu=cortex-m0 -mthumb -g -Ofast -ffp-contract=fast -funroll-loops -Wall -ffunction-sections -fdata-sections
CFLAGS      += $(INC) $(DEFS) -std=gnu99
ASFLAGS     = -mcpu=cortex-m0 -mthumb -g -Wall

LDFLAGS     = -mcpu=cortex-m0 -mthumb -nostartfiles -Wl,--gc-sections
LDFLAGS     += -T link.ld --specs=nano.specs -Wl,-Map=build/dhry_72m.map -u _printf_float

TARGET      = build/dhry_72m.elf
HEX         = build/dhry_72m.hex
BIN         = build/dhry_72m.bin

.PHONY: all clean size

all: $(TARGET)
	$(SIZE) $(TARGET)
	$(OBJCOPY) -O ihex   $(TARGET) $(HEX)
	$(OBJCOPY) -O binary $(TARGET) $(BIN)

$(TARGET): $(OBJS) link.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

build/startup_mm32f0140.o: startup_mm32f0140.s
	@mkdir -p $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c -o $@ $<

build/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: $(HAL_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

build/system_mm32f0140.o: $(DEV_DIR)/Source/system_mm32f0140.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build

size: $(TARGET)
	$(SIZE) $(TARGET)