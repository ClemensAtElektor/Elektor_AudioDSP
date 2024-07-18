Elektor Audio DSP FX Processor as 1 kHz I²S Sine Generator.

This simple example shows how to generate a sound on the ESP32 and play it through the ADAU1701 I²S slave.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Install library arduino-audio-tools from https://github.com/pschatzmann/arduino-audio-tools/
- Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect headphones or amplifier to K3 and enjoy
