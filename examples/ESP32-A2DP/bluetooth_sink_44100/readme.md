Elektor Audio DSP FX Processor as Bluetooth Sink.

This example shows how to use the ADAU1701 as I²S slave.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Install library ESP32-A2DP from https://github.com/pschatzmann/ESP32-A2DP
- Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect audio source to Bluetooth device "Elektor AudioDsp"
- Connect headphones or amplifier to K3, start audio source and enjoy
