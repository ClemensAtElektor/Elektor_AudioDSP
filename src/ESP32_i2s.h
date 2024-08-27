/*
 * Purpose: ESP32 I2S class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_I2S_H__
#define __ESP32_I2S_H__

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/

#ifdef I2S_USE_LEGACY_DRIVER
  #define CONFIG_I2S_SUPPRESS_DEPRECATE_WARN  'y' /* For ESP32 core 3.0.0 and up. */
  #define CONFIG_ADC_SUPPRESS_DEPRECATE_WARN  'y' /* The legacy I2S driver includes legacy ADC types. */
  #include <driver/i2s.h>
#else /* I2S_USE_LEGACY_DRIVER */
  #include <driver/i2s_std.h>
#endif /* I2S_USE_LEGACY_DRIVER */

class ESP32_i2s
{
private:
#ifdef I2S_USE_LEGACY_DRIVER
  i2s_port_t _i2s_port;
  i2s_config_t _i2s_config;
  i2s_pin_config_t _i2s_pin_config;
#else
  i2s_chan_handle_t _tx_handle;
  i2s_chan_handle_t _rx_handle;
  i2s_std_config_t _config;
  i2s_chan_config_t _channel_config;
#endif

public:

  ESP32_i2s(i2s_port_t i2s_port=I2S_NUM_0) :
#ifdef I2S_USE_LEGACY_DRIVER
    _i2s_port(i2s_port)
#else
	_tx_handle(NULL),
	_rx_handle(NULL)
#endif
  {
  }

  ~ESP32_i2s(void)
  {
  }

  // I2S pins must be signed, otherwise I2S_PIN_NO_CHANGE will not be recognized.
  void begin(int mck, int bck, int ws, int data_out, int data_in, uint32_t sample_rate, uint8_t bits_per_sample, i2s_port_t i2s_port=I2S_NUM_0)
  {
#ifdef I2S_USE_LEGACY_DRIVER
    port_set(i2s_port);
    pin_config_set(mck,bck,ws,data_out,data_in);
	config_set(sample_rate,bits_per_sample);
#else
    _channel_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO,I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&_channel_config,&_tx_handle,&_rx_handle));
    config_set(sample_rate,bits_per_sample,mck,bck,ws,data_out,data_in);
#endif
  }

#ifdef I2S_USE_LEGACY_DRIVER
  void print(void) const
  {
    Serial.printf("I2S port: %d\n",_i2s_port);
    Serial.printf("I2S uses the following pins:\n");
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

  void config_set(uint32_t sample_rate, uint8_t bits_per_sample)
  {
    _i2s_config.mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX);
    _i2s_config.sample_rate = sample_rate;
    _i2s_config.bits_per_sample = (i2s_bits_per_sample_t) bits_per_sample;
    _i2s_config.channel_format = (i2s_channel_fmt_t) I2S_CHANNEL_FMT_RIGHT_LEFT;
    _i2s_config.communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_MSB);
    _i2s_config.intr_alloc_flags = 0; // default interrupt priority
    _i2s_config.dma_buf_count = 8; 
    _i2s_config.dma_buf_len = 64; // deprecated
    _i2s_config.use_apll = true;
    _i2s_config.tx_desc_auto_clear = true; // avoiding noise in case of data unavailability
    _i2s_config.fixed_mclk = 256*sample_rate;
  }
  
#else
	
  void print(void) const
  {
    Serial.printf("I2S port: %d\n",_channel_config.id);
    Serial.printf("I2S uses the following pins:\n");
    Serial.printf("  - BCLK/BCK: %d\n", _config.gpio_cfg.bclk);
    Serial.printf("  - WCLK/LCK: %d\n", _config.gpio_cfg.ws);
    Serial.printf("  - DOUT: %d\n", _config.gpio_cfg.dout);
    Serial.printf("  - DIN: %d\n", _config.gpio_cfg.din);
    Serial.printf("  - MCLK: %d\n", _config.gpio_cfg.mclk);
    Serial.printf("I2S interface configuration:\n");
    Serial.printf("  - slot mode: %d\n",_config.slot_cfg.slot_mode);              
    Serial.printf("  - sample rate: %d\n",_config.clk_cfg.sample_rate_hz);
    Serial.printf("  - bits per sample: %d\n",_config.slot_cfg.data_bit_width);
    Serial.printf("  - dma buf count: %d\n",_channel_config.dma_desc_num);
    Serial.printf("  - dma buf len: %d\n",_channel_config.dma_frame_num);
    Serial.printf("  - dma auto clear: %d\n",_channel_config.auto_clear);
    Serial.printf("  - mclk multiplier: %d\n",_config.clk_cfg.mclk_multiple);
  }

  void config_set(uint32_t sample_rate, uint8_t bits_per_sample, int mclk, int bclk, int ws, int dout, int din)
  {
    i2s_std_config_t config = 
    {
      .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate), // This sets MCLK multiplier to 256.
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG((i2s_data_bit_width_t)bits_per_sample,I2S_SLOT_MODE_STEREO),
      .gpio_cfg = 
	    {
        .mclk = (gpio_num_t) mclk,
        .bclk = (gpio_num_t) bclk,
        .ws = (gpio_num_t) ws,
        .dout = (gpio_num_t) dout,
        .din = (gpio_num_t) din,
        .invert_flags = 
		    {
          .mclk_inv = false,
          .bclk_inv = false,
          .ws_inv = false,
        },
      },
    };
    _config = config;
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(_tx_handle,&_config));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(_rx_handle,&_config));
  }
#endif

  bool start(bool print_config=false)
  {
#ifdef I2S_USE_LEGACY_DRIVER
    i2s_driver_install(_i2s_port,&_i2s_config,0,NULL);
    i2s_set_pin(_i2s_port,&_i2s_pin_config);
    //i2s_set_sample_rates(_i2s_port,_i2s_pin_config.sample_rate);
    i2s_start(_i2s_port);
#else
    if (_tx_handle==NULL) return false;
    ESP_ERROR_CHECK(i2s_channel_enable(_tx_handle));
#endif
    if (print_config==true) print();
    return true;
  }
  
};


#endif /* __ESP32_I2S_H__ */
