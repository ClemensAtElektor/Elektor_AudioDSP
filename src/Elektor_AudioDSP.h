/*
 * Purpose: Audio DSP FX Processor Board classes.
 * Board: ESP32-PICO-KIT a.k.a. ESP32 PICO-D4
 * IDE: 1.8.19
 *
 * Usage:
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef ELEKTOR_AUDIO_DSP_H__
#define ELEKTOR_AUDIO_DSP_H__


#include <Arduino.h>

// This definition can be used to identify the board in applications.
#define BOARD_ELEKTOR_AUDIO_DSP

#include <stdint.h>

#include <Wire.h>
#define ADAU1701_I2C_ADDRESS  (0x68) /* 0x68 ... 0x6f (including R/W bit) */

#include "Eeprom_24c256.h"
#include "adau1701_e2prom_collection.h"
#include "ESP32_i2s.h"
#include "ESP32_adc.h"
#include "ESP32_dac.h"
#include "ESP32_dac_mux.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE  (44100)
#endif

#ifndef BITS_PER_SAMPLE
#define BITS_PER_SAMPLE  (16)
#endif

// ESP32-Pico-Kit pin definitions.
#define I2S_WS  (14)
#define I2S_BCK  (27)
#define I2S_MCK  (0) /* May be 0, 1 or 3, we use 0. */
#define I2S_DATA_OUT  (18)
#define I2S_DATA_IN1  (23)
#define POTENTIOMETERS  (4)
#define POTENTIOMETER1  (35) /* ADC1, ch7 */
#define POTENTIOMETER2  (34) /* ADC1, ch6 */
#define POTENTIOMETER3  (39) /* ADC1, ch3 */
#define POTENTIOMETER4  (36) /* ADC1, ch0 */

// MIDI definitions.
//#define MIDI  Serial1
#define MIDI_BAUD_RATE  (31250)


class Elektor_AudioDSP
{
private:
  int _led1;
  int _sda;
  int _scl;
  int _midi_in;
  int _midi_out;
  int _dsp_reset;
  int _dsp_mp[12];
  int _dsp_wp;
  int _dsp_addr1;
  int _mux0;
  int _mux1;

  // Override this function to do advanced configuration that requires the DSP to be in reset.
  virtual bool configure(void)
  {
    return true;
  }

public:
  enum
  {
    P1 = 0,
    P2 = 1,
    P3 = 2,
    P4 = 3,
	MP9 = 0, // ADAU1701 ADC0 -> MUX Y0
	MP2 = 1, // ADAU1701 ADC1 -> MUX X1
	MP3 = 2, // ADAU1701 ADC2 -> MUX X0
	MP8 = 3, // ADAU1701 ADC3 -> MUX Y1
  };
  
  ESP32_adc potentiometer[POTENTIOMETERS];
  ESP32_dac_mux dac;
  HardwareSerial& midi = Serial1;

  Elektor_AudioDSP(void) :
    _led1(2),
    _sda(9),
    _scl(10),
    _midi_in(38),
    _midi_out(4),
    _dsp_reset(12),
    _dsp_mp{I2S_DATA_OUT, I2S_DATA_IN1, POTENTIOMETER2, POTENTIOMETER3, I2S_WS, I2S_BCK, 32, 33, POTENTIOMETER4, POTENTIOMETER1, 13, 19},
    _dsp_wp(21),
    _dsp_addr1(22),
    _mux0(5),
    _mux1(15)
  {
  }

  virtual ~Elektor_AudioDSP(void) {}

  void led(bool on)
  {
    digitalWrite(_led1,on==true?HIGH:LOW);
  }

  void dsp_reset(bool assert)
  {
    digitalWrite(_dsp_reset,assert==true?LOW:HIGH); // active low
  }

  bool dsp_firmware_write_to_eeprom(uint8_t *p_data, size_t data_size)
  {
    Eeprom_24c256 eeprom = Eeprom_24c256();

    if (eeprom.compare(p_data,data_size)==false)
    {
      // EEPROM content is different, replace it.
      // Disable EEPROM write protect.
      Serial.println("Writing new firmware to EEPROM.");
      pinMode(_dsp_wp,OUTPUT);
      digitalWrite(_dsp_wp,LOW);
	  // Write EEPROM.
      eeprom.write(p_data,data_size);
      // Assert write protect.
      digitalWrite(_dsp_wp,HIGH);
      return (eeprom.compare(p_data,data_size));
    }
    Serial.println("Keeping existing EEPROM contents.");
    return true;
  }

  bool dsp_is_available(uint8_t i2c_addr=ADAU1701_I2C_ADDRESS)
  {
    Wire.beginTransmission(i2c_addr>>1);
    if (Wire.endTransmission()==0) return true;
    return false;
  }

  void dsp_halt(void)
  {
	// Force DSP in reset.
    pinMode(_dsp_reset,OUTPUT);
    dsp_reset(true);
  }

  bool dsp_run(void)
  {
    // Release DSP from reset
    dsp_reset(false);
	// Give the DSP some time to initialize.
    delay(100);

    // See if the DSP is responding.
    if (dsp_is_available()==false)
    {
      Serial.printf("DSP not found on I2C bus. Did you set JP1 correctly?\n");
	  return false;
    }
	
	return true;
  }
  
  bool begin(uint8_t *p_eeprom_data=NULL, size_t eeprom_data_size=0, bool release_dsp=true)
  {
	// Force DSP in reset.
	dsp_halt();

    // Make sure this is the very first I2C call, even before Wire.begin.
    Wire.setPins(_sda,_scl);
	// Start I2C bus.
    Wire.begin();
    Wire.setClock(400000);

	// LED on.
    pinMode(_led1,OUTPUT);
	led(true);

    // Configure the poteniometers (i.e., the ADC).
    potentiometer[0].begin(_dsp_mp[9],ADC1_CHANNEL_7); // MP9, ADAU1701 ADC0
    potentiometer[1].begin(_dsp_mp[2],ADC1_CHANNEL_6); // MP2, ADAU1701 ADC1
    potentiometer[2].begin(_dsp_mp[3],ADC1_CHANNEL_3); // MP3, ADAU1701 ADC2
    potentiometer[3].begin(_dsp_mp[8],ADC1_CHANNEL_0); // MP8, ADAU1701 ADC3
	
    // Configure the DAC multiplexer.
	dac.begin(_mux0,_mux1);

    // Configure MIDI I/O.
    midi.begin(MIDI_BAUD_RATE,SERIAL_8N1,_midi_in,_midi_out);

    // Write (new) firmware to EEPROM (if any).
    if (p_eeprom_data!=NULL && eeprom_data_size!=0)
	{
      if (dsp_firmware_write_to_eeprom(p_eeprom_data,eeprom_data_size)==false)
      {
        Serial.println("Failed to write DSP firmware to EEPROM.");
        return false;
      }
	}

    // Do initialization stuff here that requires the DSP to be in reset.
    if (configure()==false)
    {
      Serial.println("Configuration during DSP reset failed");
	  return false;
    }

    // Release DSP from reset
    if (release_dsp==true)
    {
      if (dsp_run()==false) return false;
      // Everything went well, turn off the LED.
      led(false); // LED off.
    }

	return true;
  }

};


class Elektor_AudioDSP_I2S_Out : public Elektor_AudioDSP
{
private:
  ESP32_i2s _i2s;
  // I2S pins must be signed, otherwise I2S_PIN_NO_CHANGE will not be recognized.
  int _mck;
  int _bck;
  int _ws;
  int _data_out;
  int _data_in;
  uint32_t _sample_rate;
  uint8_t _bits_per_sample;

  bool configure(void) override
  {
    // I2S MCLK is used to clock the DSP, so setup I2S first.
    // Serial.println("Starting I2S");
    _i2s.begin(_mck,_bck,_ws,_data_out,_data_in,_sample_rate,_bits_per_sample);
	return true;
  }

public:

  Elektor_AudioDSP_I2S_Out(uint32_t sample_rate=SAMPLE_RATE, uint8_t bits_per_sample=BITS_PER_SAMPLE) :
    _mck(I2S_MCK),
    _bck(I2S_BCK),
    _ws(I2S_WS),
    _data_out(I2S_DATA_OUT),
    _data_in(I2S_PIN_NO_CHANGE),
    _sample_rate(sample_rate),
    _bits_per_sample(bits_per_sample)
  {
  }

  i2s_port_t i2s_port_get(void) const
  {
    return _i2s.port_get();
  }
  
  i2s_config_t i2s_config_get(void) const
  {
    return _i2s.config_get();
  }

  i2s_pin_config_t i2s_pin_config_get(void) const
  {
    return _i2s.pin_config_get();
  }

  void i2s_config_print(void) const
  {
    _i2s.print();
  }

  void i2s_start(bool print_config=false)
  {
    _i2s.start(print_config);
  }
  
};


#endif /* ELEKTOR_AUDIO_DSP_H__ */
