/*
 * Purpose: Elektor Audio DSP FX Processor as MIDI sender/receiver (example)
 *          - Prints MIDI data received on K5 to serial port.
 *          - Sends Key On / Key Off messages on MIDI out (K5).
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Connect MIDI keyboard/controller to K5
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Send MIDI data
 *
 * By: Clemens Valens
 * Date: 09/07/2024
 */

#include "Elektor_AudioDSP.h"
Elektor_AudioDSP audiodsp = Elektor_AudioDSP();


void setup(void) 
{
  Serial.begin(115200); // For debugging, etc.
  
  audiodsp.begin(); // Not uploading any firmware for DSP.

  Serial.println("Waiting for MIDI data...");
}

void loop(void)
{
  static uint32_t half_seconds = 0;
  static uint32_t t_prev = millis();

  // Print incoming MIDI data. Expected length is 3 bytes, other lengths
  // may mess up output formatting.
  if (audiodsp.midi.available()>2)
  {
    while (audiodsp.midi.available())
    {
      uint8_t ch = audiodsp.midi.read();
      Serial.printf("%02x ",ch);
    }
    Serial.println();
  }

  if (millis()>t_prev+500)
  {
    t_prev = millis();
    half_seconds += 1;
    audiodsp.led(half_seconds&0x01); // Toggle LED.
    
    if ((half_seconds&0x01)==0)
    {
      // MIDI out - note on
      audiodsp.midi.write(0x90);
      audiodsp.midi.write(0x55);
      audiodsp.midi.write(127);
    }
    else
    {
      // MIDI out - note off.
      audiodsp.midi.write(0x80);
      audiodsp.midi.write(0x55);
      audiodsp.midi.write(0);
    }
  }
}
