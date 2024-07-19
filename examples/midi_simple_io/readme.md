Elektor Audio DSP FX Processor as MIDI sender/receiver (example)
- Prints MIDI data received on K5 to serial port.
- Sends Key-On / Key-Off messages on MIDI out (K5).

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Usage:
- Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Connect MIDI keyboard/controller to K5
- Upload this sketch to the ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Send MIDI data
