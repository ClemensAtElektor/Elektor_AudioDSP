/*
 * Purpose: Elektor Audio DSP FX Processor as ADAU1701 Sine Generator (example)
 *          The DSP runs a VCO algorithm controlled by P1 (mapped to MP9) with
 *          its output connected to K4. 
 *          I²S pass-through is active too (on K3) but not used in this sketch.
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Use P5 to adjust TP1 to 3.15 V
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect headphones or amplifier to K4 (not K3!) and enjoy
 * - P1 controls frequency
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#define SAMPLE_RATE  (48000)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP audiodsp = Elektor_AudioDSP();

#include "adau1701_e2prom_collection.h"
// Yes, this is the correct firmware. It has a VCO output on K4.
#define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_I2S_48000_SIZE
#define ADAU1701_FIRMWARE  adau1701_firmware_i2s_pass_through_48000


void setup(void) 
{
  Serial.begin(115200); // For debugging, etc.
  
  audiodsp.begin(ADAU1701_FIRMWARE,ADAU1701_FIRMWARE_SIZE);

  Serial.println("Started, sine wave on K4 (not K3!), P1 controls frequency.");
}

void loop(void)
{
  static uint32_t half_seconds = 0;
  static uint32_t t_prev = millis();

  uint16_t p1 = audiodsp.potentiometer[audiodsp.P1].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP9]!=p1) audiodsp.dac[audiodsp.MP9] = p1; 
  uint16_t p2 = audiodsp.potentiometer[audiodsp.P2].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP2]!=p2) audiodsp.dac[audiodsp.MP2] = p2; 
  uint16_t p3 = audiodsp.potentiometer[audiodsp.P3].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP3]!=p3) audiodsp.dac[audiodsp.MP3] = p3; 
  uint16_t p4 = audiodsp.potentiometer[audiodsp.P4].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP8]!=p4) audiodsp.dac[audiodsp.MP8] = p4; 
  audiodsp.dac.refresh(); // Call regularly.
  
  if (millis()>t_prev+500)
  {
    t_prev = millis();
    half_seconds += 1;
    audiodsp.led(half_seconds&0x01); // Toggle LED.
    // Show potentiometer values.
    Serial.printf("%d\t%d\t%d\t%d\n",p1,p2,p3,p4);
  }
}
