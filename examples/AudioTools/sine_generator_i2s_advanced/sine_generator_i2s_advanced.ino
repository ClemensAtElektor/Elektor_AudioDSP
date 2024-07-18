/*
 * Purpose: Elektor Audio DSP FX Processor as I2S Sine Generator with frequency & amplitude control (example)
 *          - P1 controls frequency
 *          - P2 controls amplitude
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Install library arduino-audio-tools from https://github.com/pschatzmann/arduino-audio-tools/
 * - Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Use P5 to adjust TP1 to 3.15 V
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect headphones or amplifier to K3 and enjoy
 * - P1 controls frequency
 * - P2 controls amplitude
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#define SAMPLE_RATE  (48000)
#define BITS_PER_SAMPLE  (32)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP_I2S_Out audiodsp = Elektor_AudioDSP_I2S_Out(SAMPLE_RATE,BITS_PER_SAMPLE);

#include "adau1701_e2prom_collection.h"
#define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_I2S_48000_SIZE
#define ADAU1701_FIRMWARE  adau1701_firmware_i2s_pass_through_48000

// Requires library arduino-audio-tools from https://github.com/pschatzmann/arduino-audio-tools/
#include "AudioTools.h"
AudioInfo info(SAMPLE_RATE,2,BITS_PER_SAMPLE);
SineWaveGenerator<int32_t> sineWave(0x7fffffff);
GeneratedSoundStream<int32_t> sound(sineWave); // Stream generated from sine wave
I2SStream out; 
StreamCopy copier(out,sound); // copies sound into i2s

void setup(void) 
{
  Serial.begin(115200); // For debugging, etc.
  
  // Do not release the DSP at the end of begin (third parameter = false),
  // because I2S must be configured and started first.
  audiodsp.begin(ADAU1701_FIRMWARE,ADAU1701_FIRMWARE_SIZE,false);

  // Start I2S.
  Serial.println("Starting I2S");
  // Get the default I2S configuration from the library.
  auto config = out.defaultConfig(TX_MODE);
  config.copyFrom(info);

  // Reconfigure I2S pins for our board.
  i2s_pin_config_t i2s_pin_config = audiodsp.i2s_pin_config_get();
  config.pin_mck = i2s_pin_config.mck_io_num;
  config.pin_data = i2s_pin_config.data_out_num;
  config.pin_ws = i2s_pin_config.ws_io_num;
  config.pin_bck = i2s_pin_config.bck_io_num;

  // Setup I2S format & MCLK for our board.
  i2s_config_t i2s_config = audiodsp.i2s_config_get();
  config.i2s_format = I2S_MSB_FORMAT;
  config.fixed_mclk = i2s_config.fixed_mclk; // fixed_mclk *must* be 256*sample_rate
  out.begin(config);

  // The DSP is clocked by the ESP32's I2S MCLK signal. Therefore,
  // I2S must be running before the DSP can run.
  audiodsp.dsp_run();
  
  // Setup 1 kHz sine wave.
  sineWave.begin(info,1000);
  Serial.println("Started: sine wave on K3; P1 controls frequency; P2 sets amplitude");
}

void loop(void)
{
  static uint32_t half_seconds = 0;
  static uint32_t t_prev = millis();
  static uint32_t t_prev_potentiometers = millis();
  
  if (millis()>t_prev_potentiometers+100)
  {
    t_prev_potentiometers = millis();
    uint16_t p1 = audiodsp.potentiometer[audiodsp.P1].read();
    float f = (float)p1*1900.0/4096.0+100.0; // 100 ... 2000 Hz
    sineWave.setFrequency(f);
    
    uint16_t p2 = audiodsp.potentiometer[audiodsp.P2].read();
    float a = (float)p2/4096.0; // a = 0 ... 1.0
    // sineWave is declared at the top as int32_t, meaning that the
    // amplitude is supposed to be in the range -max_int32_t to +max_int32_t
    // It would have been more elegant if the arduino-audio-library did this conversion for us.
    sineWave.setAmplitude(a*NumberConverter::maxValueT<int32_t>());
    
    //Serial.printf("P1=%d\tf=%0.1f\t",p1,f);
    //Serial.printf("P2=%d\ta=%0.1f\n",p2,a);
  }
  
  if (millis()>t_prev+500)
  {
    t_prev = millis();
    half_seconds += 1;
    audiodsp.led(half_seconds&0x01); // Toggle LED.
  }
  
  // Copy sound to output.
  copier.copy();
}