/*
 * Purpose: ESP32 DAC multiplexer class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_DAC_MUX_H__
#define __ESP32_DAC_MUX_H__


#include "ESP32_dac.h"
#include <stdexcept>

#define MUX_CHANNELS  (4)

class ESP32_dac_mux
{
private:
  ESP32_dac _dac[2];
  uint8_t _mux0;
  uint8_t _mux1;
  uint8_t _value[MUX_CHANNELS];
  bool _dirty[MUX_CHANNELS];
  
public:

  ESP32_dac_mux(void) :
    _value{0, 0, 0, 0},
	_dirty{true, true, true, true},
	_mux0(0xff),
	_mux1(0xff)
  {
  }
  
  void begin(uint8_t mux0, uint8_t mux1)
  {
	// Initialize pins.
	_mux0 = mux0;
	_mux1 = mux1;
    pinMode(_mux0,OUTPUT);
    pinMode(_mux1,OUTPUT);
	// Initialize DACs.
    _dac[0].begin(0); // DAC channel 1
    _dac[1].begin(1); // DAC channel 2
	// Force update.
    refresh(true);
  }
  
  void refresh_channel(uint8_t value1, uint8_t value2, uint8_t channel)
  {
    // The mux has 2 channels (X & Y) of 1:2 multiplexers.
    digitalWrite(_mux0,channel);
    digitalWrite(_mux1,channel);
    _dac[0].write(value1); // X
    _dac[1].write(value2); // Y
    delay(20);
  }

  void refresh(bool force=false)
  {
    if (_dirty[0]==true || _dirty[2]==true || force==true)
	{
      refresh_channel(_value[2],_value[0],LOW); // X0, Y0
	  _dirty[0] = false;
	  _dirty[2] = false;
    }
    if (_dirty[1]==true || _dirty[3]==true || force==true)
	{
      refresh_channel(_value[1],_value[3],HIGH); // X1, Y1
	  _dirty[1] = false;
	  _dirty[3] = false;
	}
  }

  void write(uint8_t channel, uint8_t value)
  {
    if (channel<MUX_CHANNELS)
    {
      if (_value[channel]!=value)
      {
        _value[channel] = value;
        _dirty[channel] = true;
      }
    }
  }

  uint8_t& operator[](uint8_t channel)
  {
    if (channel>=MUX_CHANNELS)
    {
      throw std::out_of_range("dac_mux channel out of range");;
	}
    _dirty[channel] = true;
    return _value[channel];
  }

  const uint8_t& operator[](uint8_t channel) const
  {
    if (channel>=MUX_CHANNELS)
    {
      throw std::out_of_range("dac_mux channel out of range (const)");;
	}
    return _value[channel];
  }
  
};


#endif /* __ESP32_DAC_MUX_H__ */