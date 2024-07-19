Elektor Audio DSP FX Processor as Pitch Shifter (example)

The DSP runs the pitchshifter block from SigmaStudio:

- Frequency shifting is controlled by P1 (mapped to MP9 aka ADC0).
- Mono, only the right audio channel is used.
- Input connected to K2.
- Output connected to K3.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Use P5 to adjust TP1 to 3.15 V
- Upload this sketch to the ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect sound source to K2 (right channel only)
- Connect headphones or amplifier to K3 and enjoy (right channel only)
- P1 controls frequency shift
