# i2c_ee_test - onboard 24C02 EEPROM speed test (MM32F0144C6P)

Tests the onboard **24C02** I2C EEPROM (SCL = PB6, SDA = PB7) on the
MINI-F0144 board and measures erase / program / read throughput.

The 24C02 is driven with a software (bit-bang) I2C master on PB6/PB7
(open-drain with the onboard pull-ups), 100 kHz nominal SCL.

## Behavior

1. Scan the I2C bus 0x50..0x5F to locate the EEPROM
2. **Erase**: write 0xFF to all 256 bytes (8-byte pages, tWR delay each)
3. **Program**: write `addr+i` pattern to all 256 bytes
4. **Read**: read back all 256 bytes
5. Verify readback matches the programmed pattern
6. Report per-operation time (ms) and throughput (B/s)

## Result (measured)

```
I2C address scan (0x50..0x5F):
  found EEPROM at 7-bit 0x50

Erase  time: 704 ms
Program time: 704 ms
Read   time: 417 ms
Erase  speed: 363 B/s
Program speed: 363 B/s
Read   speed: 613 B/s
Readback match: PASS
```

## Build

```sh
make clean
make
```

Outputs: `build/i2c_ee_test.elf`, `.bin`, `.hex`.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/i2c_ee_test.hex`)

## Notes

- Throughput is dominated by the 24C02 write-cycle time (tWR ~5 ms/page)
  and the software I2C bit-bang; erase/program hence take ~0.7 s for the
  full 256-byte device.
- The onboard 24C02 is at 7-bit address 0x50.
- Console output over UART2 (PA2 Tx) at 115200 baud.