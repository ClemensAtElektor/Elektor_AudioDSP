Elektor Audio DSP FX Processor signal generator illustrating how to control DSP algorithms running on the ADAU1701 over I²C.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Install library SigmaDSP from https://github.com/MCUdude/SigmaDSP
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 away from 'Selfboot' (i.e. *not* Selfboot, pins 1 & 2)
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect headphones or amplifier to K3 and enjoy

Adapted from SigmaDSP library example 5_Signal_generator.
