/*
 * Purpose: ESP32 DAC class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_DAC_H__
#define __ESP32_DAC_H__


#include <driver/dac.h>

class ESP32_dac
{
private:
  dac_channel_t _channel;

public:

  ESP32_dac(void) :
    _channel(static_cast<dac_channel_t>(0xff)) // Use static_cast for explicit type
  {
  }
  
  ~ESP32_dac(void)
  {
    if (is_available())
	{
      dac_output_disable(_channel);
	}
  }
  
  bool is_available(void) const
  {
    return _channel==DAC_CHANNEL_1 || _channel==DAC_CHANNEL_2;
  }
  
  bool begin(uint8_t channel)
  {
    if (channel==0) _channel = DAC_CHANNEL_1; // GPIO25
    else if (channel==1) _channel = DAC_CHANNEL_2; // GPIO26
    else return false;
    dac_output_enable(_channel);
    return true;
  }
  
  bool write(uint8_t value)
  {
    if (is_available())
	{
      dac_output_voltage(_channel,value);
	  return true;
	}
    return false;
  }
  
};


#endif /* __ESP32_DAC_H__ */
