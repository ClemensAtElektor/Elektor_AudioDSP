/*
 * Purpose: ESP32 ADC class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_ADC_H__
#define __ESP32_ADC_H__


#include <driver/adc.h>

class ESP32_adc
{
private:
  adc1_channel_t _channel;
  uint8_t _pin;
  uint16_t _value;
  
public:

  ESP32_adc(void) :
    _channel(static_cast<adc1_channel_t>(0xff)),
	_pin(0xff),
	_value(0)
  {
  }
  
  void begin(uint8_t pin, adc1_channel_t channel)
  {
    _pin = pin;
    _channel = channel;
    adc1_config_width(ADC_WIDTH_BIT_12);
  	adc1_config_channel_atten(_channel,ADC_ATTEN_DB_11);
  }
  
  uint16_t read(void)
  {
    _value = adc1_get_raw(_channel);
    return _value;
  }
  
  uint16_t read_previous(void) const
  {
    return _value;
  }
  
};


#endif // __ESP32_ADC_H__
