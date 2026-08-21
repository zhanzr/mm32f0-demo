# blink_hello - bare-metal demo for MINI-F0144

Minimal bare-metal project for the **MINI-F0144** (MM32F0144C6P), built with
`arm-none-eabi-gcc`.

## Features

- Blinks the 4 on-board LEDs: PA15, PB3, PB4, PB5 (low active), every 1 s
- Prints system clock over UART2 (PA2 TXd -> bridge RxD, 115200)
- Reports ADC internal channels every 5 s: VREF (ch15) -> voltage,
  die temperature (ch14) -> Celsius, VDDA derived from Vref calibration

Example console output:

```
[5] SYSCLK=72000000Hz (72.000MHz)  HCLK=72000000Hz  VDDA=3.36V  VREF[ch15]=1523 -> 1.25V  TEMP[ch14]=1687 -> 32.7C
```

## Build

```sh
make clean
make
```

Outputs go to `build/`: `blink_hello.elf`, `blink_hello.bin`, `blink_hello.hex`.

## Flash

The board is externally powered; the probe only provides SWD. The default
(and only working) flasher for this MCU is **pyOCD**:

```sh
pyocd load -t mm32f0144c6p --pack <MM32F0140_DFP.pack> -O reset_type=sw build/blink_hello.hex
```

The `MM32F0140_DFP.pack` comes from the MindMotion device family pack
(`D:\Arm\Packs\MindMotion\MM32F0140_DFP\<ver>` — package its `Flash/` +
`SVD/` + `.pdsc` into a zip, pyOCD loads it via `--pack`).

## Toolchain comparison

Benchmarked on this board (ULINK2 as CMSIS-DAP, 30,720-byte image):

| Tool | Erase + program | Differential reflash | Status |
| ---- | --------------- | -------------------- | ------ |
| pyOCD | 7.55 s | 2.45 s | default, reliable |
| OpenOCD | n/a | n/a | cannot flash (unsupported device ID in stm32f0x driver) |
| probe-rs | n/a | n/a | cannot flash (no MM32 chip/flash-algo definition) |

OpenOCD and probe-rs attach to the core but have no MM32F0140 flash
algorithm support out of the box; they are not used for flashing in this
project. pyOCD is therefore the default and required method.

## Notes

Do not reconfigure PA13/PA14 as GPIO while debugging — they are the SWD
pins, and doing so locks out the debugger until a power cycle.