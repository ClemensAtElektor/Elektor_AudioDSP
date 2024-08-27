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


// Don't forget to define your ESP32 module before including this file, i.e.
//   #define ESP32_MODULE  ESP32_PICO_KIT_D4
// of
//   #define ESP32_MODULE  ESP32_PICO_KIT_1
#define ESP32_PICO_KIT_D4  (1)
#define ESP32_PICO_KIT_1  (2)

// If you want to get rid of warnings about which module will be used, 
// define MODULE_TYPE_WARNING_OFF before including this library.
//#define MODULE_TYPE_WARNING_OFF

#ifndef ESP32_MODULE
  #ifndef MODULE_TYPE_WARNING_OFF
    #warning "ESP32_MODULE undefined, using ESP32_PICO_KIT_D4"
  #endif
  #define ESP32_MODULE  ESP32_PICO_KIT_D4
#endif


#include <Arduino.h>

// This definition can be used to identify the board in applications.
#define BOARD_ELEKTOR_AUDIO_DSP

#include <stdint.h>

#include <Wire.h>
#define ADAU1701_I2C_ADDRESS  (0x68) /* 0x68 ... 0x6f (including R/W bit) */

#include "Eeprom_24c256.h"
#include "adau1701_e2prom_collection.h"

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/
#define I2S_USE_LEGACY_DRIVER
#include "ESP32_i2s.h"

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/
#define ADC_USE_LEGACY_DRIVER
#include "ESP32_adc.h"

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/
#define DAC_USE_LEGACY_DRIVER
#include "ESP32_dac.h"
#include "ESP32_dac_mux.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE  (44100)
#endif

#ifndef BITS_PER_SAMPLE
#define BITS_PER_SAMPLE  (16)
#endif

// ESP32-Pico-Kit pin definitions.
#define AUDIODSP_LED1  (2)
#define MIDI_IN  (38)
#define MIDI_OUT  (4)
#define DSP_RESET  (12)
#define DSP_WP  (21)
#define DSP_ADDR1  (22)
#define DAC_MUX0  (5)
#define DAC_MUX1  (15)

#if ESP32_MODULE == ESP32_PICO_KIT_D4
  #ifndef MODULE_TYPE_WARNING_OFF
    #warning "Compiling for ESP32_PICO_KIT_D4"
  #endif
  #define I2C_SDA  (9)
  #define I2C_SCL  (10)
  #define DSP_MP0  (18)
  #define DSP_MP1  (23)
#elif ESP32_MODULE == ESP32_PICO_KIT_1
  #ifndef MODULE_TYPE_WARNING_OFF
    #warning "Compiling for ESP32_PICO_KIT_1"
  #endif
  #define I2C_SDA  (9)
  #define I2C_SCL  (10)
  #define DSP_MP0  (7)
  #define DSP_MP1  (8)
#else
  #error "Invalid ESP32 module selection."
#endif

#define DSP_MP2  (34)
#define DSP_MP3  (39)
#define DSP_MP4  (14)
#define DSP_MP5  (27)
#define DSP_MP6  (32)
#define DSP_MP7  (33)
#define DSP_MP8  (36)
#define DSP_MP9  (35)
#define DSP_MP10  (13)
#define DSP_MP11  (19)
#define I2S_WS  DSP_MP4
#define I2S_BCK  DSP_MP5
#define I2S_MCK  (0) /* May be 0, 1 or 3, we use 0. */
#define I2S_DATA_OUT  DSP_MP0
#define I2S_DATA_IN1  DSP_MP6

#define POTENTIOMETERS  (4)
#define POTENTIOMETER1  DSP_MP9 /* ADC1, ch7 */
#define POTENTIOMETER2  DSP_MP2 /* ADC1, ch6 */
#define POTENTIOMETER3  DSP_MP3 /* ADC1, ch3 */
#define POTENTIOMETER4  DSP_MP8 /* ADC1, ch0 */

// MIDI definitions.
#define MIDI_PORT  Serial1
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
  HardwareSerial& midi = MIDI_PORT;

  Elektor_AudioDSP(void) :
    _led1(AUDIODSP_LED1),
    _sda(I2C_SDA),
    _scl(I2C_SCL),
    _midi_in(MIDI_IN),
    _midi_out(MIDI_OUT),
    _dsp_reset(DSP_RESET),
    _dsp_mp{I2S_DATA_OUT, I2S_DATA_IN1, POTENTIOMETER2, POTENTIOMETER3, I2S_WS, I2S_BCK, DSP_MP6, DSP_MP7, POTENTIOMETER4, POTENTIOMETER1, DSP_MP10, DSP_MP11},
    _dsp_wp(DSP_WP),
    _dsp_addr1(DSP_ADDR1),
    _mux0(DAC_MUX0),
    _mux1(DAC_MUX1)
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

  bool e2prom_write(uint8_t *p_data, size_t data_size)
  {
    if (dsp_firmware_write_to_eeprom(p_data,data_size)==true)
    {
      Serial.println("Done.");
      return true;
    }
    return false;
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
      Serial.printf("DSP not found on I2C bus. Is JP1 (DSP clock) positioned correctly?\n");
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
    potentiometer[0].begin(_dsp_mp[9],ADC_CHANNEL_7); // MP9, ADAU1701 ADC0
    potentiometer[1].begin(_dsp_mp[2],ADC_CHANNEL_6); // MP2, ADAU1701 ADC1
    potentiometer[2].begin(_dsp_mp[3],ADC_CHANNEL_3); // MP3, ADAU1701 ADC2
    potentiometer[3].begin(_dsp_mp[8],ADC_CHANNEL_0); // MP8, ADAU1701 ADC3
	
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
#ifdef I2S_USE_LEGACY_DRIVER
    _data_in(I2S_PIN_NO_CHANGE),
#else
    _data_in(I2S_GPIO_UNUSED),
#endif
    _sample_rate(sample_rate),
    _bits_per_sample(bits_per_sample)
  {
  }

#ifdef I2S_USE_LEGACY_DRIVER
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
#endif

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
