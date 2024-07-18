Elektor Audio DSP FX Processor as ADAU1701 Sine Generator.

In this example, the DSP runs a SigmaStudion VCO algorithm. Its frequency is controlled by P1, which is read by the ESP32 and forwarded to MP9 through the board's DAC multiplexer.
 
The output in on K4 (not K3!). I²S pass-through is active too (on K3) but not used in this sketch.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Use P5 to adjust TP1 to 3.15 V
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect headphones or amplifier to K4 (not K3!) and enjoy
- P1 controls frequency
