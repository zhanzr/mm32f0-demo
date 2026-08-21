# mm32f0-demo

Bare-metal demo projects for MindMotion MM32F0 boards.

Boards live in their own folder; each project is independent and has its
own build instructions:

- `mini-f0144/` — MINI-F0144 board (MM32F0144C6P)
  - `bare/blink_hello/` — blink + console, arm-none-eabi-gcc
  - `bare/dhry_72m/` — Dhrystone 2.1 benchmark @ 72 MHz, arm-none-eabi-gcc

Shared dependency: the vendor HAL library in `LibSamples_MM32F0140_V1.9.11/`.

See each project README for build and flash instructions.