/*
 * Purpose: Elektor Audio DSP FX Processor as Pitch Shifter (example)
 *          - The DSP runs the pitchshifter block from SigmaStudio
 *          - Frequency shift is controlled by P1 (mapped to MP9 aka ADC0)
 *          - Input (right channel only) on K2
 *          - Output on K3 (right channel only)
 *
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Use P5 to adjust TP1 to 3.15 V
 * - Upload this sketch to the ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect sound source to K2 (right channel only)
 * - Connect headphones or amplifier to K3 and enjoy (right channel only)
 * - P1 controls frequency shift
 *
 * By: Clemens Valens
 * Date: 10/07/2024
 */

#define SAMPLE_RATE  (48000)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP audiodsp = Elektor_AudioDSP();

#include "adau1701_e2prom_collection.h"
#define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_PITCHSHIFTER_SIZE
#define ADAU1701_FIRMWARE  adau1701_firmware_pitchshifter


void setup(void) 
{
  Serial.begin(115200); // For debugging, etc.
  
  audiodsp.begin(ADAU1701_FIRMWARE,ADAU1701_FIRMWARE_SIZE);

  Serial.println("Started, P1 controls frequency shifting.");
}

void loop(void)
{
  static uint32_t half_seconds = 0;
  static uint32_t t_prev = millis();

  uint16_t p1 = audiodsp.potentiometer[audiodsp.P1].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP9]!=p1) audiodsp.dac[audiodsp.MP9] = p1; // Only update when needed.
  audiodsp.dac.refresh(); // Call regularly.
  
  if (millis()>t_prev+500)
  {
    t_prev = millis();
    half_seconds += 1;
    audiodsp.led(half_seconds&0x01); // Toggle LED.
  }
}
