# mm32f0-demo

Bare-metal demo projects for MindMotion MM32F0 boards.

| Board | Directory | MCU |
| ----- | --------- | --- |
| MINI-F0144 | `mini-f0144/` | MM32F0144C6P |

Each board folder contains its own projects; each project is independent and
has its own build instructions. Shared dependencies in the repo:

- Vendor HAL library: `LibSamples_MM32F0140_V1.9.11/`
- CMSIS device packs for flashing: `packs/`