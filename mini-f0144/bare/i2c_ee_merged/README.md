# i2c_ee_merged - onboard 24C02 EEPROM test (hardware + software I2C)

Merged test project for the onboard **24C02** EEPROM on the MINI-F0144 board
(SCL = PB6, SDA = PB7). Runs **both** interfaces in one binary, measuring
erase / program / read throughput for each:

1. **Hardware I2C** - official HAL I2C1 master (PB6/PB7, AF1, 100 kHz)
   with an ACK probe and per-operation timeouts so a flaky bus can never hang.
2. **Software I2C (bit-bang)** - GPIO open-drain drives the same pins
   (~100 kHz nominal), reliable baseline.

Both tests verify a full 256-byte readback against the programmed pattern.

## Result (measured, repeated loop)

```
--- Hardware I2C (24C02: 256 B) ---
Erase  time: 1601 ms    speed: 159 B/s
Program time: 1600 ms    speed: 160 B/s
Read   time: 28 ms      speed: 9142 B/s
Readback match: PASS

--- Software I2C (bit-bang) (24C02: 256 B) ---
Erase  time: 673 ms    speed: 380 B/s
Program time: 672 ms    speed: 380 B/s
Read   time: 427 ms    speed: 599 B/s
Readback match: PASS
```

## Observations

- **Hardware I2C** reads far faster (multiple bytes per transaction,
  ~9000 B/s vs ~600 B/s), but its writes appear slower (~160 B/s vs
  ~380 B/s) because the driver waits ~50 ms per page write (tWR).
- The DesignWare I2C master on this part is **intermittently unreliable**:
  it sometimes fails to ACK / stalls on reads. This project therefore probes
  for an ACK first and guards every wait with a timeout. The bit-bang path
  is the dependable one used as the baseline.

## Build

```sh
make clean
make
```

Outputs: `build/i2c_ee_merged.elf`, `.bin`, `.hex`.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/i2c_ee_merged.hex`)

## Notes

- Console output over UART2 (PA2 Tx) at 115200 baud.
- Onboard 24C02 is at 7-bit address 0x50 (8-bit write address 0xA0).
- This project replaces the former separate `i2c_ee_test` (bit-bang) and
  `i2c_ee_test_hw` (hardware) demo projects.