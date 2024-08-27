Elektor Audio DSP FX Processor as Bluetooth Sink.

This example shows how to use the ADAU1701 as I²S slave.

Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
IDE: 1.8.19

Compile with ESP32 core 2.0.17 or older. Do not use core V3.0.0 or newer.
The reason are the new I2S, ADC & DAC drivers that are not compatible with
the now "legacy" drivers. Building this example for core 3.0.0 and up with 
the new drivers will result in an executable that doesn't fit in the chip.

Make sure to define A2DP_LEGACY_I2S_SUPPORT before including the Bluetooth library:

#define A2DP_LEGACY_I2S_SUPPORT  (1)
#include "BluetoothA2DPSink.h"
BluetoothA2DPSink a2dp_sink = BluetoothA2DPSink();


Usage:
- Install library ESP32-A2DP from https://github.com/pschatzmann/ESP32-A2DP
- Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
- Set JP2 to position 'Selfboot' (pins 2 & 3)
- Upload this sketch to ESP32
- Wait for LED1 to start blinking at a rate of 1 Hz
- Connect audio source to Bluetooth device "Elektor AudioDsp"
- Connect headphones or amplifier to K3, start audio source and enjoy
