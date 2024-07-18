/*
 * Purpose: ESP32 I2S class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_I2S_H__
#define __ESP32_I2S_H__


#include <driver/i2s.h>

class ESP32_i2s
{
private:
  i2s_port_t _i2s_port;
  i2s_config_t _i2s_config;
  i2s_pin_config_t _i2s_pin_config;

public:

  ESP32_i2s(i2s_port_t i2s_port=I2S_NUM_0) :
    _i2s_port(i2s_port)
  {
  }

  ~ESP32_i2s(void)
  {
  }

  // I2S pins must be signed, otherwise I2S_PIN_NO_CHANGE will not be recognized.
  void begin(int mck, int bck, int ws, int data_out, int data_in, uint32_t sample_rate, uint8_t bits_per_sample, i2s_port_t i2s_port=I2S_NUM_0)
  {
    port_set(i2s_port);
    pin_config_set(mck,bck,ws,data_out,data_in);
	config_set(sample_rate,bits_per_sample);
  }

  void print(void) const
  {
    Serial.printf("I2S port: %d\n");
    Serial.printf("I2S uses the following pins:\n",_i2s_port);
    Serial.printf("  - BCLK/BCK: %d\n", _i2s_pin_config.bck_io_num);
    Serial.printf("  - WCLK/LCK: %d\n", _i2s_pin_config.ws_io_num);
    Serial.printf("  - DOUT: %d\n", _i2s_pin_config.data_out_num);
    Serial.printf("  - DIN: %d\n", _i2s_pin_config.data_in_num);
    Serial.printf("  - MCLK: %d\n", _i2s_pin_config.mck_io_num);
    Serial.printf("I2S interface configuration:\n");
    Serial.printf("  - mode: %d\n",_i2s_config.mode);              
    Serial.printf("  - sample rate: %d\n",_i2s_config.sample_rate);
    Serial.printf("  - bits per sample: %d\n",_i2s_config.bits_per_sample);
    Serial.printf("  - channel format: %d\n",_i2s_config.channel_format);
    Serial.printf("  - communication format: %d\n",_i2s_config.communication_format);
    Serial.printf("  - intr alloc flags: %d\n",_i2s_config.intr_alloc_flags);
    Serial.printf("  - dma buf count: %d\n",_i2s_config.dma_buf_count);
    Serial.printf("  - dma buf len: %d\n",_i2s_config.dma_buf_len);
    Serial.printf("  - use apll: %d\n",_i2s_config.use_apll);
    Serial.printf("  - tx desc auto clear: %d\n",_i2s_config.tx_desc_auto_clear);
    Serial.printf("  - fixed mclk: %d\n",_i2s_config.fixed_mclk);
  }
  
  i2s_port_t port_get(void) const
  {
    return _i2s_port;
  }
  
  void port_set(i2s_port_t i2s_port)
  {
    _i2s_port = i2s_port;
  }
  
  i2s_config_t config_get(void) const
  {
    return _i2s_config;
  }
  
  void config_set(uint32_t sample_rate, uint8_t bits_per_sample)
  {
    _i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX);
    _i2s_config.sample_rate = sample_rate;
    _i2s_config.bits_per_sample = (i2s_bits_per_sample_t) bits_per_sample;
    _i2s_config.channel_format = (i2s_channel_fmt_t) I2S_CHANNEL_FMT_RIGHT_LEFT;
    _i2s_config.communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_MSB);
    _i2s_config.intr_alloc_flags = 0; // default interrupt priority
    _i2s_config.dma_buf_count = 8;
    _i2s_config.dma_buf_len = 64;
    _i2s_config.use_apll = true;
    _i2s_config.tx_desc_auto_clear = true; // avoiding noise in case of data unavailability
    _i2s_config.fixed_mclk = 256*sample_rate;
  }
  
  i2s_pin_config_t pin_config_get(void) const
  {
    return _i2s_pin_config;
  }
  
  // I2S pins must be signed, otherwise I2S_PIN_NO_CHANGE will not be recognized.
  void pin_config_set(int mck, int bck, int ws, int data_out, int data_in)
  {
    _i2s_pin_config.mck_io_num = mck;
    _i2s_pin_config.bck_io_num = bck;
    _i2s_pin_config.ws_io_num = ws;
    _i2s_pin_config.data_out_num = data_out;
    _i2s_pin_config.data_in_num = data_in;
  }

  void start(bool print_config=false)
  {
    i2s_driver_install(_i2s_port,&_i2s_config,0,NULL);
    i2s_set_pin(_i2s_port,&_i2s_pin_config);
    //i2s_set_sample_rates(_i2s_port,_i2s_pin_config.sample_rate);
    i2s_start(_i2s_port);
    if (print_config==true) print();
  }
  
};


#endif /* __ESP32_I2S_H__ */
