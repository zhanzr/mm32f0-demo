# i2c_ee_test_hw - official hardware I2C EEPROM demo (MM32F0144C6P)

Clone of the vendor **I2C_Master_EEPROM_Polling** example
(`Samples/LibSamples/I2C/I2C_Master_EEPROM_Polling`), used to verify the
**hardware I2C1** driver against the onboard **24C02** (SCL = PB6, SDA = PB7).

Keeps the official code intact (HAL I2C1 on PB6/PB7, 100 kHz); only the
console UART was moved to PA2/PA3 and a full-device speed benchmark added.

## Does the vendor example really work?

**Yes.** Compiled with arm-none-eabi-gcc and run on the board, the official
hardware I2C EEPROM polling example works correctly:

- EEPROM Write: OK
- Readback: `0x30 0x31 ... 0x42 0x43` (expected)
- Full 256-byte erase/program/readback: **PASS**

## Result (measured)

```
EEPROM Write : OK
EEPROM Read  : 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39
               0x3a 0x3b 0x3c 0x3d 0x3e 0x3f 0x40 0x41 0x42 0x43

=== full 24C02 (256 B) speed ===
Erase  time: 1601 ms
Program time: 1600 ms
Read   time: 30 ms
Erase  speed: 159 B/s
Program speed: 160 B/s
Read   speed: 8533 B/s
Readback match: PASS
```

## Speed comparison

See `../i2c_ee_test/README.md` for the bit-bang baseline comparison.

## Build

Built with `arm-none-eabi-gcc` (GCC startup + linker + newlib), compiling the
official HAL I2C demo sources:

```sh
make clean
make
```

Outputs: `build/i2c_ee_test_hw.elf`, `.bin`, `.hex`.

> Note: the code also compiles with Keil ARM Compiler 6 (armclang/armlink),
> but the default ARM toolchain triggers platform/ABI quirks; the GCC build
> is the supported one in this repo for reliable flashing.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/i2c_ee_test_hw.hex`)

## Notes

- Console output over UART2 (PA2 Tx) at 115200 baud.
- The onboard 24C02 is at 7-bit address 0x50 (write addr 0xA0).