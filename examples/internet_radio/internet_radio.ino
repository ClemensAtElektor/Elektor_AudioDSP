/*
 * Purpose: Elektor Audio DSP FX Processor as internet radio.
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'ext. MCLK' (pins 2 & 3)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Enter your Wi-Fi settings in the file credentials.h
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect headphones or amplifier to K3
 * - Turn P1 to change stations (up to three but you can add more)
 *
 * Adapted from a project found online (lost the link :-( ), but it seems to be based 
 * on this repository: https://github.com/schreibfaul1/ESP32-audioI2S
 * 
 * By: Clemens Valens
 * Date: 11/07/2024
 */

#define SAMPLE_RATE  (44100)
//#define SAMPLE_RATE  (48000)
#define BITS_PER_SAMPLE  (16)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP_I2S_Out audiodsp = Elektor_AudioDSP_I2S_Out(SAMPLE_RATE,BITS_PER_SAMPLE);

#include "adau1701_e2prom_collection.h"
#if SAMPLE_RATE==44100
  #define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_I2S_44100_SIZE
  #define ADAU1701_FIRMWARE  adau1701_firmware_i2s_pass_through_44100
#elif SAMPLE_RATE==48000
  #define ADAU1701_FIRMWARE_SIZE  ADAU1701_FIRMWARE_I2S_48000_SIZE
  #define ADAU1701_FIRMWARE  adau1701_firmware_i2s_pass_through_48000
#else
  #error "SAMPLE_RATE not valid."
#endif

#include "Audio.h"
// Create an instance of audio class & install I2S driver.
Audio audio(I2S_BCK,I2S_WS,I2S_DATA_OUT,SAMPLE_RATE);

#include "WiFi.h"
// Wi-Fi network settings, enter your SSID and password in the file credentials.h
#include "credentials.h"
const char* ssid = MY_SSID;
const char* password = MY_PASSWORD;

void change_station(uint8_t station)
{ 
  static uint8_t station_prev = 0xff;
  
  if (station_prev==station) return; // Do nothing.
  
  String station_str;
  switch (station)
  {
    case 0:
      // Worked at the time of writing.
      station_str = "http://149.13.0.81/nova128";
      break;
	  
    case 1: 
      // Worked at the time of writing.
      station_str = "http://ice1.somafm.com/groovesalad-128-mp3";
      break;
	  
    case 2:
      // Worked at the time of writing.
      station_str = "http://rtlberlin.streamabc.net/rtlb-80rock-mp3-192-1947757";
      break;

    default: 
      return;
  }

  audio.connecttohost(station_str);
  station_prev = station;
  Serial.print("Connecting to ");
  Serial.println(station_str);
}

void setup(void)
{
  Serial.begin(115200);

  // Do not release the DSP at the end of begin (third parameter: false), 
  // because WiFi & I2S must be configured and started first.
  audiodsp.begin(ADAU1701_FIRMWARE,ADAU1701_FIRMWARE_SIZE,false);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  Serial.printf("Connecting to network ");
  while (WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf(" ok\n");

  audio.setPinout(I2S_BCK,I2S_WS,I2S_DATA_OUT);
  audio.i2s_mclk_pin_select(I2S_MCK);    
  audio.setSampleRate(SAMPLE_RATE);
  audio.setBitsPerSample(BITS_PER_SAMPLE);
  audio.setChannels(2); 
  audio.setVolume(20);  // range 0-21

  // The DSP is clocked by the ESP32's I2S MCLK signal. Therefore,
  // I2S must be running before the DSP can run.
  audiodsp.dsp_run();

  Serial.printf("Starting audio...\n"); 
}

void loop(void)
{
  static uint32_t rate = 31; // ms, 31 results in approx. 1 Hz LED toggle rate.
  static uint32_t milliseconds = 0;
  static uint32_t t_prev = millis();
  
  audio.audiotask(); // Audio task, run often.

  // User interface.  
  if (millis()>t_prev+rate)
  {
    t_prev = millis();
    milliseconds += rate;
    
    audiodsp.led((milliseconds>>4)&0x01); // Toggle LED.
    
    // Use P1 to choose between three stations.
    uint16_t p1 = audiodsp.potentiometer[audiodsp.P1].read();
    if (p1<1300) p1 = 0;
    else if (p1>1400 && p1<2600) p1 = 1;
    else if (p1>2700) p1 = 2;
    if (p1<3) change_station(p1);
  }
}
