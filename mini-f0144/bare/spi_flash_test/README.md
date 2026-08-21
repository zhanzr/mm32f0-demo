# spi_flash_test - ZD25WQ80 SPI NOR flash speed test (MM32F0144C6P)

Tests the onboard **ZD25WQ80** SPI NOR flash (1 MB) on the MINI-F0144 board
and measures erase / program / read throughput.

SPI connection on **SPI2**:

| Signal | Pin |
| ------ | --- |
| CS  (SS) | PB12 |
| SCK      | PB13 |
| MISO     | PB14 |
| MOSI     | PB15 |

Driver style: polling byte-wise SPI master (based on the vendor
`SPI_Master_FLASH_Polling` example), SCK prescaler `/2`
(SPI2 in = PCLK1 = 36 MHz, so SCK ~18 MHz — the fastest SPI2 can go).
The test region is 16 KB (4 sectors).

## Behavior (repeating loop)

1. Read JEDEC ID
2. **Erase**: sector-erase (0x20) the 16 KB region (4 x 4 KB sectors)
3. **Program**: page-program (0x02) with a byte pattern (64 x 256 B pages)
4. **Read**: fast-read (0x0B) the region
5. Verify readback equals the programmed pattern, report PASS/FAIL

## Result (measured)

```
JEDEC ID: 0xBA6014

--- SPI Flash speed (region 16 KB, SPI2 prescaler /2) ---
Erase  time: 35 ms    speed: 468114 B/s   (~457 KiB/s)
Program time: 130 ms    speed: 126030 B/s (~123 KiB/s)
Read   time: 43 ms    speed: 381023 B/s   (~372 KiB/s)
Readback match: PASS
```

## Build

```sh
make clean
make
```

Outputs: `build/spi_flash_test.elf`, `.bin`, `.hex`.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/spi_flash_test.hex`)

## Notes

- SPI2 clock = PCLK1 = 36 MHz; with prescaler `/2` SCK ~18 MHz (max for
  SPI2). The byte-wise polling driver limits effective throughput well
  below SCK; DMA or larger FIFO bursts would push read closer to the bus
  limit.
- Prescaler sweep measured on this board (all readback PASS):
  `/16` -> 169 KB/s read, `/4` -> 309 KB/s, `/2` -> ~375 KB/s.
- JEDEC ID `0xBA6014` is read from the ZD25WQ80.
- Console output over UART2 (PA2 Tx) at 115200 baud.
- Changing `SPI_BaudRatePrescaler` in `spi_flash_test.c` adjusts the bus
  speed (e.g. `/256` is much slower, ~13 KB/s read).