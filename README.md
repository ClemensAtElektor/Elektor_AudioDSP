# Elektor Audio DSP FX Processor

The Elektor Audio DSP FX Processor is a board combining an ESP32-PICO-KIT with an ADAU1701 Aduio DSP as intelligent programmable I²S CODEC. Basically, it is an ESP32 microcontroller with high-quality audio in- and outputs added to it. Basically, as it has much more to offer than just that. What sets the Audio DSP FX Processor apart from other, somewhat similar (on first sight) boards is that the audio codec (from COder/DECoder, in other words, the ADC & DAC) integrates a DSP capable of processing audio all by itself. This handy feature makes the board not only powerful, but also super flexible and versatile.

**Specifications**

- ADAU1701 28-/56-bit, 50 MIPS digital audio processor supporting sampling rates of up to 192 kHz.
- ESP32-PICO-D4 32-bit dual-core microcontroller with Wi-Fi 802.11b/g/n and Bluetooth 4.2 BR/EDR and BLE
- 2× 24-bit audio inputs (2 VRMS, 20 kΩ)
- 4× 24-bit audio outputs (0.9 VRMS, 600 Ω)
- 4× control potentiometer
- MIDI in- and output
- I²C expansion port
- Multi-mode operation
- Power supply: 5 VDC USB or 7.5 VDC – 12 VDC (barrel jack, center pin is GND) 

**Applications**

- Bluetooth/Wi-Fi audio sink (e.g. loudspeaker) & source
- Guitar effect pedal (stomp box)
- Music synthesizer
- Sound/function generator
- Programmable cross-over filter for loudspeakers
- Advanced audio effects processor (reverb, pitch shifting, etc.)
- Internet-connected audio device
- DSP experimentation platform
- Wireless MIDI
- MIDI to CV
- Etc.

**Installation**

This library can be installed from the Arduino IDE's library manager, just search on Elektor Audio DSP FX Processor.
You can also copy this folder with all its files into the 'libraries' folder of the 'Sketchbook' folder:

[path]/[to]/Sketchbook/libraries/Elektor_AudioDSP

Certain examples and functionalities require third-party libraries that must be installed separately:

- [arduino-audio-tools](https://github.com/pschatzmann/arduino-audio-tools/)
- [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP)
- [ML_SynthTools](https://github.com/marcel-licence/ML_SynthTools)
- [SigmaDSP](https://github.com/MCUdude/SigmaDSP)
  
**Usage**

Examples illustrating different aspects of the board are included. Note that certain examples require third-party libraries that must be installed separately. Therefore, carefull read the documentation proviede with each example.

The version of the ESP32 core installed in Arduino is important. As of ESP32 core version 3.0.0, certain drivers have been deprecated ('legacy') and have been replaced by new ones. Examples are the DAC, ADC and I²S drivers. The legacy drivers are supported until Espressif decides to drop them completely. The Elektor_AudioDSP is intended for use with both legacy and new drivers. However, some examples will only work with legacy drivers and an ESP32 core 2.0.x. Legacy drivers can be enabled or disabled by using some #define, e.g. #define ADC_USE_LEGACY_DRIVER.

For easy switching between multiple ESP32 cores, it is highly recommended to use Arduino IDE 1.8.x instead of 2.x. The Portable feature of the 1.8.x IDE makes this possible.

Another confusing Espressif thing is the large number of ESP32 variants. This project is intended for the ESP32-PICO-D4. The ESP32-PICO-V3 can also be used, but not the ESP32-PICO-V3-02 nor the ESP32-PICO-Mini because they lack GPIO pins 9 and 10.  

**About**

Board: ESP32 PICO KIT + ADAU1701

Developped on Arduino IDE 1.8.19 with ESP32 core 2.0.17. Compile for board ESP32 PICO-D4
