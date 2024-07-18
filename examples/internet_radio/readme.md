Elektor Audio DSP FX Processor as internet radio

The DSP functions as I²S output:
- Output connected to K3.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect headphones or amplifier to K3 and enjoy

Adapted from a project found online (lost the link), but it seems to be based on this repository: https://github.com/schreibfaul1/ESP32-audioI2S
