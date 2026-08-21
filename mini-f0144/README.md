# MINI-F0144 Board

MindMotion **MM32F0144C6P** development board.

![MINI-F0144 board](board_images/board_0.png)

## Features

| Item | Description |
| ---- | ----------- |
| MCU | MM32F0144C6P (Cortex-M0, max 72 MHz) |
| Flash | 64 KiB |
| SRAM | 8 KiB |
| LEDs | PA15, PB3, PB4, PB5 (low active) |
| UART | UART2 on PA2 (Txd) / PA3 (Rxd), or UART1 on PA9 / PA10 |
| I2C EEPROM | 24C02 on PB6 (SCL) / PB7 (SDA) |
| Debug | SWD (via CMSIS-DAP probe, externally powered) |

## Projects

Each project is independent and lives in its own folder under `bare/`:

- `bare/blink_hello/` — bare-metal blink + console demo (arm-none-eabi-gcc)
- `bare/dhry_72m/` — Dhrystone 2.1 benchmark @ 72 MHz (arm-none-eabi-gcc)
- `bare/pwm_test/` — passive buzzer PWM sweep on PA8 + LED duty pattern (arm-none-eabi-gcc)
- `bare/i2c_ee_test/` — onboard 24C02 EEPROM speed test (arm-none-eabi-gcc)

## Programming

The board is powered externally (the debug probe does **not** power the
target). Use pyOCD over SWD; the MM32F0140 device family pack is vendored in
the repo at `packs/MindMotion.MM32F0140_DFP.pack`, so each project flashes
with a single command:

```sh
cd bare/<project>
make flash
```

Console output: UART2 PA2 (Txd) to bridge RxD, 115200 baud.