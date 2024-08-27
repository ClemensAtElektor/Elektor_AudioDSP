/*
 * Purpose: ESP32 DAC class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_DAC_H__
#define __ESP32_DAC_H__

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/

#ifdef DAC_USE_LEGACY_DRIVER
  #define CONFIG_DAC_SUPPRESS_DEPRECATE_WARN  'y' /* For ESP32 core 3.0.0 and up. */
  #include <driver/dac.h>
#else /* DAC_USE_LEGACY_DRIVER */
  #include <driver/dac_oneshot.h>
#endif /* DAC_USE_LEGACY_DRIVER */


class ESP32_dac
{
private:
  dac_channel_t _channel;
#ifndef DAC_USE_LEGACY_DRIVER
  dac_oneshot_handle_t _handle;
#endif

public:

  ESP32_dac(void) :
#ifndef DAC_USE_LEGACY_DRIVER
    _handle(NULL),
#endif
    _channel(static_cast<dac_channel_t>(0xff)) // Use static_cast for explicit type
  {
  }
  
  ~ESP32_dac(void)
  {
    if (is_available())
    {
#ifdef DAC_USE_LEGACY_DRIVER
      dac_output_disable(_channel);
#else
      dac_oneshot_del_channel(_handle);
      _handle = NULL;
#endif
    }
  }
  
  bool is_available(void) const
  {
#ifdef DAC_USE_LEGACY_DRIVER
    return _channel==DAC_CHANNEL_1 || _channel==DAC_CHANNEL_2;
#else
    return _handle!=NULL;
#endif
  }
  
  bool begin(uint8_t channel)
  {
#ifdef DAC_USE_LEGACY_DRIVER
    if (channel==0) _channel = DAC_CHANNEL_1; // GPIO25
    else if (channel==1) _channel = DAC_CHANNEL_2; // GPIO26
    else return false;
    dac_output_enable(_channel);
#else
    if (channel==0) _channel = DAC_CHAN_0; // GPIO25
    else if (channel==1) _channel = DAC_CHAN_1; // GPIO26
    else return false;
	
    if (_handle==NULL)
    {
      dac_oneshot_config_t config = 
      {
        .chan_id = _channel,
      };
      // This will allocate and enable the DAC.
      ESP_ERROR_CHECK(dac_oneshot_new_channel(&config,&_handle));
    }
#endif

    return is_available();
  }
  
  bool write(uint8_t value)
  {
    if (is_available())
    {
#ifdef DAC_USE_LEGACY_DRIVER
      dac_output_voltage(_channel,value);
#else
      ESP_ERROR_CHECK(dac_oneshot_output_voltage(_handle,value));
#endif
      return true;
    }
    return false;
  }
  
};


#endif /* __ESP32_DAC_H__ */
