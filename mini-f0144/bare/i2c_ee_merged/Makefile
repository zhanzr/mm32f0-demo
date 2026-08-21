# i2c_ee_merged - onboard 24C02 EEPROM test (hardware I2C + software bit-bang)
# Toolchain: arm-none-eabi-gcc

CROSS      ?= arm-none-eabi-
CC          = $(CROSS)gcc
OBJCOPY     = $(CROSS)objcopy
SIZE        = $(CROSS)size

# Vendor library root (relative to this Makefile)
LIB         ?= ../../../LibSamples_MM32F0140_V1.9.11

# CMSIS device family pack for flashing (vendored in repo /packs)
PACK        ?= ../../../packs/MindMotion.MM32F0140_DFP.pack
TARGET_CHIP  = mm32f0144c6p

DEV_DIR     = $(LIB)/Device/MM32F0140
CMSIS_DIR   = $(LIB)/Device/CMSIS/KEIL_Core
HAL_DIR     = $(DEV_DIR)/HAL_Lib/Src

APP_SRCS    = main.c platform.c i2c_ee_merged.c mm32f0140_it.c syscalls.c
HAL_SRCS    = hal_adc.c hal_comp.c hal_crc.c hal_dbg.c hal_div.c hal_dma.c \
              hal_exti.c hal_flash.c hal_flexcan.c hal_gpio.c hal_i2c.c \
              hal_iwdg.c hal_misc.c hal_pwr.c hal_rcc.c hal_spi.c hal_tim.c \
              hal_uart.c hal_uid.c hal_ver.c hal_wwdg.c

APP_OBJS    = $(addprefix build/,$(APP_SRCS:.c=.o))
HAL_OBJS    = $(addprefix build/,$(HAL_SRCS:.c=.o))
OBJS        = $(APP_OBJS) $(HAL_OBJS) build/system_mm32f0140.o build/startup_mm32f0140.o

BUILD_DIR   = build

INC         = -I. \
              -I$(DEV_DIR)/Include \
              -I$(DEV_DIR)/HAL_Lib/Inc \
              -I$(CMSIS_DIR)

DEFS        = -DUSE_STDPERIPH_DRIVER

CFLAGS      = -mcpu=cortex-m0 -mthumb -g -O2 -Wall -ffunction-sections -fdata-sections
CFLAGS      += $(INC) $(DEFS) -std=gnu99
ASFLAGS     = -mcpu=cortex-m0 -mthumb -g -Wall

LDFLAGS     = -mcpu=cortex-m0 -mthumb -nostartfiles -Wl,--gc-sections
LDFLAGS     += -T link.ld --specs=nano.specs -Wl,-Map=build/i2c_ee_merged.map

TARGET      = build/i2c_ee_merged.elf
HEX         = build/i2c_ee_merged.hex
BIN         = build/i2c_ee_merged.bin

.PHONY: all clean size flash

all: $(TARGET) $(HEX) $(BIN)
	$(SIZE) $(TARGET)

$(TARGET): $(OBJS) link.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(HEX): $(TARGET)
	$(OBJCOPY) -O ihex   $< $@

$(BIN): $(TARGET)
	$(OBJCOPY) -O binary $< $@

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

flash: $(HEX)
	pyocd load -t $(TARGET_CHIP) --pack $(PACK) -O reset_type=sw $(HEX)