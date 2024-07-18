Elektor Audio DSP FX Processor as simple stereo chorus

The DSP runs the chorus block from SigmaStudio:

- Input connected to K2.
- Output connected to K3.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Use P5 to adjust TP1 to 3.15 V
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect sound source to K2
- Connect headphones or amplifier to K3 and enjoy
