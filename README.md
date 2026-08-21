# mm32f0-demo

Bare-metal demo projects for MindMotion MM32F0 boards.

Boards live in their own folder:

- `mini-f0144/` — MINI-F0144 board (MM32F0144C6P)
  - `bare/` — bare-metal blink + console, arm-none-eabi-gcc

Shared dependency: the vendor HAL library in `LibSamples_MM32F0140_V1.9.11/`.

## Build

```sh
cd mini-f0144/bare
make
```

## Flash

Flashing is done with **pyOCD** over a CMSIS-DAP-class probe (e.g. Keil ULINK2). See `mini-f0144/bare/README.md` for the full command and for the other toolchain options.