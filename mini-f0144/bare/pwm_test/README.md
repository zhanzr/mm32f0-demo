# pwm_test - passive buzzer PWM sweep (MM32F0144C6P)

Drives the on-board passive buzzer via **TIM1_CH1 PWM on PA8** (2700 Hz, the
frequency where the buzzer is most audible) and sweeps the duty cycle while
showing the duty on the LEDs and reporting it over UART.

Built with `arm-none-eabi-gcc`.

## Behavior

Main loop (repeats forever):

1. Set PWM duty rate and start PWM output
2. Increase duty by 5%, wrap into `[0, 100)`
3. Update the LED pattern and print the duty over UART
4. On wrap (new rate < old rate) delay 5 s more, then continue the loop
5. Otherwise delay 500 ms
6. Stop PWM output
7. Delay 250 ms

### LED pattern (duty -> LEDs)

| Duty range | LEDs |
| ---------- | ---- |
| [0, 20)    | all off |
| [20, 40)   | LD1 |
| [40, 60)   | LD1, LD2 |
| [60, 80)   | LD1, LD2, LD3 |
| [80, 100)  | all on |

LEDs are low-active (on == pin low).

## Build

```sh
make clean
make
```

Outputs: `build/pwm_test.elf`, `.bin`, `.hex`.

## Flash

The MM32F0140 device family pack is vendored at
`packs/MindMotion.MM32F0140_DFP.pack`, so flashing is a single command:

```sh
make flash
```

(equivalent to `pyocd load -t mm32f0144c6p --pack ../../../packs/MindMotion.MM32F0140_DFP.pack -O reset_type=sw build/pwm_test.hex`)

## Console output

Over UART2 (PA2 Tx, 115200):

```
PWM duty = 5 %
PWM duty = 10 %
...
PWM duty = 95 %
PWM duty = 0 %     <- wrap, +5 s hold
PWM duty = 5 %
...
```

## Hardware verification

TIM1_CH1 configured for 2700 Hz PWM on PA8 (`GPIO_AF_2`). Verified by reading
the timer registers over SWD:

- `ARR = 0x6829 (26665)` -> 72 MHz / (26666) ~= **2700 Hz**
- `CCR1 = 1333` -> ~**5%** duty
- `CCER.CC1E = 1` -> channel output enabled