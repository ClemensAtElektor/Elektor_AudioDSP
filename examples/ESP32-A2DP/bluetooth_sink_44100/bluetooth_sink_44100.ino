/*
 * Purpose: Elektor Audio DSP FX Processor as Bluetooth Sink (example)
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 * 
 * ! Compile with ESP32 core 2.0.17 or older. Do not use core V3.0.0 or newer.
 * ! The reason are the new I2S, ADC & DAC drivers that are not compatible with
 * ! the now "legacy" drivers. Building this example for core 3.0.0 and up with 
 * ! the new drivers will result in an executable that doesn't fit in the chip.
 *
 * Usage:
 * - Install library ESP32-A2DP from https://github.com/pschatzmann/ESP32-A2DP
 * - Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect audio source to Bluetooth device "Elektor AudioDsp"
 * - Connect headphones or amplifier to K3, start audio source and enjoy
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#define ESP32_MODULE  ESP32_PICO_KIT_1
#define SAMPLE_RATE  (44100)
#define BITS_PER_SAMPLE  (16)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP_I2S_Out audiodsp = Elektor_AudioDSP_I2S_Out(SAMPLE_RATE,BITS_PER_SAMPLE);

#define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_I2S_44100_SIZE
#define ADAU1701_FIRMWARE  adau1701_firmware_i2s_pass_through_44100

// Requires library ESP32-A2DP: https://github.com/pschatzmann/ESP32-A2DP
// VID (Very Important Definition)
#define A2DP_LEGACY_I2S_SUPPORT  (1)
#include "BluetoothA2DPSink.h"
BluetoothA2DPSink a2dp_sink = BluetoothA2DPSink();


void setup(void)
{
  Serial.begin(115200); // For debugging, etc.
  
  // Do not release the DSP at the end of begin (third parameter: false), 
  // because I2S must be configured and started first.
  audiodsp.begin(ADAU1701_FIRMWARE,ADAU1701_FIRMWARE_SIZE,false);

  // Do I2S setup.
  a2dp_sink.set_pin_config(audiodsp.i2s_pin_config_get());
  a2dp_sink.set_i2s_config(audiodsp.i2s_config_get());
  a2dp_sink.start("Elektor AudioDsp1");
  
  audiodsp.i2s_config_print(); // For info/porting/debugging only.

  // The DSP is clocked by the ESP32's I2S MCLK signal. Therefore,
  // I2S must be running before the DSP can run.
  audiodsp.dsp_run();
  
  Serial.println("Entering loop.");
}

void loop(void)
{
  static uint32_t half_seconds = 0;
  static uint32_t t_prev = millis();
  
  uint16_t p1 = audiodsp.potentiometer[audiodsp.P1].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP9]!=p1) audiodsp.dac[audiodsp.MP9] = p1; // 'POT4' (P1/MP9/ADC0): master volume
  uint16_t p2 = audiodsp.potentiometer[audiodsp.P2].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP2]!=p2) audiodsp.dac[audiodsp.MP2] = p2; // 'POT2' (P2/MP2/ADC2): J14 filter
  uint16_t p3 = audiodsp.potentiometer[audiodsp.P3].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP3]!=p3) audiodsp.dac[audiodsp.MP3] = p3; // 'POT3' (P3/MP3/ADC1): J13 filter 
  uint16_t p4 = audiodsp.potentiometer[audiodsp.P4].read() >> 4; // DAC is 8-bit.
  if (audiodsp.dac[audiodsp.MP8]!=p4) audiodsp.dac[audiodsp.MP8] = p4; // 'POT1' (P4/MP8/ADC3): volume (relative)
  audiodsp.dac.refresh(half_seconds&0x01); // Call regularly, force update every once in a while.

  if (millis()>t_prev+500)
  {
    t_prev = millis();
    half_seconds += 1;
    audiodsp.led(half_seconds&0x01); // Toggle LED.
    Serial.printf("%d\t%d\t%d\t%d\n",p1,p2,p3,p4);
  }
}
