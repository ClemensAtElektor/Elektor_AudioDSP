Elektor Audio DSP FX Processor as TCP/IP Server for SigmaStudio

This program allows SigmaStudio to load programs into the ADAU1701 over a TCP/IP connection.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 away from 'Selfboot' (i.e. *not* Selfboot, pins 1 & 2)
- Upload this sketch to the ESP32
- Wait for LED1 to switch off before trying to connect from SigmaStudio
