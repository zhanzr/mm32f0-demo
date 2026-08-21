# dhry_72m - Dhrystone 2.1 benchmark (MM32F0144C6P @ 72 MHz)

Dhrystone 2.1 benchmark for the **MINI-F0144** board, built with
`arm-none-eabi-gcc -Ofast -ffp-contract=fast -funroll-loops` (no LTO).

The benchmark runs repeatedly in the main loop (once per loop iteration);
LEDs toggle after each run, then a 1 s pause.

## Build

```sh
make clean
make
```

Outputs: `build/dhry_72m.elf`, `.bin`, `.hex`.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/dhry_72m.hex`)

## Benchmark result

Console output over UART2 (PA2 Tx, 115200):

```
Dhrystone Benchmark, Version 2.1 (Language: C)
Program compiled with 'register' attribute
Execution starts, 200000 runs through Dhrystone
Execution ends

MicroSecond for one run through Dhrystone:  16.110
Dhrystones per Second:   62073.246
DMIPS/MHz:   0.491

Dhrystones per Second: 62073.246 (62.1k/s)
DMIPS/MHz:             0.491
```

Measured: **Mic. sec/run = 16.110, Dhrystones/s = 62073.246, DMIPS/MHz = 0.491**
(200,000 runs, run loop stabilized, result identical across consecutive runs.)

Config: SYSCLK/HCLK = 72 MHz, ARM GCC (Cortex-M0, Thumb), `-Ofast
-ffp-contract=fast -funroll-loops`, Dhrystone v2.1 with `register`.

## Notes

- Not built with LTO (`-flto` would optimise parts of the benchmark loop out
  of the timed region).
- 8 KiB SRAM limits Dhrystone array sizes to `char` (50x50) as in the
  original v2.1 distribution.
- Requires the MM32F0140 DFP pack for pyOCD, same as `blink_hello`.