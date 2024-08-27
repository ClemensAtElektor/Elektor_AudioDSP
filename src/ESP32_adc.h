/*
 * Purpose: ESP32 ADC class
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ESP32_ADC_H__
#define __ESP32_ADC_H__

/*
ESP32 core 3.0.0 and up: use new driver if possible, the legacy driver may no longer be supported.
ESP32 core before 3.0.0: use legacy driver, the new driver is not supported.
*/

#ifdef ADC_USE_LEGACY_DRIVER

  #define CONFIG_ADC_SUPPRESS_DEPRECATE_WARN  'y' /* For ESP32 core 3.0.0 and up. */
  #include <driver/adc.h>
  #define adc_channel_t  adc1_channel_t
  #define ADC_CHANNEL_7  ADC1_CHANNEL_7
  #define ADC_CHANNEL_6  ADC1_CHANNEL_6
  #define ADC_CHANNEL_3  ADC1_CHANNEL_3
  #define ADC_CHANNEL_0  ADC1_CHANNEL_0
  
#else /* ADC_USE_LEGACY_DRIVER */

  #include <esp_adc/adc_oneshot.h>

  // Only one ADC unit is allowed for an application so keep a global handle.
  static adc_oneshot_unit_handle_t _handle = NULL;

  //#define ESP32_ADC_USE_CALIBRATION
  #ifdef ESP32_ADC_USE_CALIBRATION
    // Calibration scheme is not required when doing raw reads. 
    adc_cali_handle_t _cali_handle = NULL;
  #endif

#endif /* ADC_USE_LEGACY_DRIVER */


class ESP32_adc
{
private:
  adc_channel_t _channel;
  uint8_t _pin;
  int _value;
  
public:

  ESP32_adc(void) :
    _channel(static_cast<adc_channel_t>(0xff)),
    _pin(0xff),
    _value(0)
  {
  }
  
  void begin(uint8_t pin, adc_channel_t channel)
  {
    _pin = pin;
    _channel = channel;

#ifdef ADC_USE_LEGACY_DRIVER

    adc1_config_width(ADC_WIDTH_BIT_12);
  	adc1_config_channel_atten(_channel,ADC_ATTEN_DB_12);

#else /* ADC_USE_LEGACY_DRIVER */

    // ADC initialization, do only once per application.
    if (_handle==NULL)
    {
      adc_oneshot_unit_init_cfg_t unit_config =
      {
        .unit_id = ADC_UNIT_1,
        //.clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
      };
      ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config,&_handle));
    }

    // ADC configuration.
    adc_oneshot_chan_cfg_t channel_config = 
    {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(_handle,_channel,&channel_config));
	
#ifdef ESP32_ADC_USE_CALIBRATION
	// ADC calibration initialization.
    if (_cali_handle==NULL)
    {
      adc_cali_line_fitting_config_t cali_config = 
      {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
      };
      adc_cali_line_fitting_efuse_val_t cali_val;

      // See esp_adc/adc_cali_scheme.h - unclear documentation, not sure how to handle this situation.
      ESP_ERROR_CHECK(adc_cali_scheme_line_fitting_check_efuse(&cali_val));
      if (cali_val==ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF)
      {
        Serial.printf("ESP32_adc::begin - cali_config.default_vref needs initializing (but with what?)\n");
      }
	  
      ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config,&_cali_handle));
    }
#endif

#endif /* ADC_USE_LEGACY_DRIVER */
  }
  
  // Do not call from ISR.
  int read(void)
  {
#ifdef ADC_USE_LEGACY_DRIVER
    _value = adc1_get_raw(_channel);
    return _value;
#else /* ADC_USE_LEGACY_DRIVER */
    // Calibration scheme is not required when doing raw reads. 
    if (adc_oneshot_read(_handle,_channel,&_value)==ESP_OK) return _value;
	  return 0;
#endif /* ADC_USE_LEGACY_DRIVER */
  }
  
  int read_previous(void) const
  {
    return _value;
  }
  
};


#endif // __ESP32_ADC_H__
