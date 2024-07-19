/*
 * Purpose: Elektor Audio DSP FX Processor signal generator (example)
 *          illustrating how to control an ADAU1701 program over I²C.
 * Board: ESP32 PICO-D4 (on Elektor Audio DSP FX Processor Board)
 * IDE: 1.8.19
 *
 * Usage:
 * - Install library SigmaDSP from https://github.com/MCUdude/SigmaDSP
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 away from 'Selfboot' (i.e. *not* Selfboot, pins 1 & 2)
 * - Upload this sketch to ESP32
 * - Wait for LED1 to start blinking at a rate of 1 Hz
 * - Connect headphones or amplifier to K3 and enjoy
 *
 * Adapted from 5_Signal_generator for Elektor board.
 * Date: 09/07/2024
 */

/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 5_Signal_generator.ino                       |
| This example we use the DSP as a crude       |
| Signal generator.                            |
| See the SigmaStudio project file if you want |
| to learn more, tweak or do modifications.    |
|**********************************************/

// Elektor Audio DSP FX Processor pins.
const int i2c_sda = 9;
const int i2c_scl = 10;
const int dsp_reset = 12;
const int led1 = 2;


// Include Wire and SigmaDSP library
#include <Wire.h>
#include <SigmaDSP.h>

// Include generated parameter file
#include "SigmaDSP_parameters.h"

// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the sample rate
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire,DSP_I2C_ADDRESS,48000.00f,dsp_reset);

// Only needed if an external i2c EEPROM is present + the DSP is in selfboot mode
// The first parameter is the Wire object we'll be using when communicating wth the EEPROM
// The second parameter is the EEPROM i2c address, which is defined in the parameter file
// The third parameter is the EEPROM size in kilobits (kb)
// An optional fourth parameter is the pin to toggle while writing content to EEPROM
//DSPEEPROM ee(Wire, EEPROM_I2C_ADDRESS, 256, LED_BUILTIN);

#define WAVEFORMS  (4)

void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Signal generator example\n"));

  pinMode(led1,OUTPUT);
  digitalWrite(led1,HIGH);
  
  // Make sure this called before Wire.begin
  Wire.setPins(i2c_sda,i2c_scl);
  Wire.begin();
  dsp.begin();
  //ee.begin();

  delay(2000);

  Serial.println(F("Pinging i2c bus...\n0 -> device is present\n2 -> device is not present"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp.ping());
  //Serial.print(F("EEPROM ping: "));
  //Serial.println(ee.ping());

  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println("Done!\n");

  // Comment out the three code lines above and use this step instead if EEPROM is present
  // The last parameter in writeFirmware is the FW version, and prevents the MCU from overwriting on every reboot
  //ee.writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 0);
  //dsp.reset();
  //delay(2000); // Wait for the FW to load from the EEPROM

  // Set volume to -30dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR,0);

  digitalWrite(led1,LOW);
}

const uint16_t step_max = 300;
const uint16_t f_min = 50;
const uint16_t f_max = 19000;

void loop(void)
{
  static uint8_t wave = 0;
  static uint16_t step = 0;

  digitalWrite(led1,step&0x0010); // Toggle led.

  if (step==0)
  {
    dsp.mux(MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR,wave);
    if (wave==0)
  	{
      Serial.print(F("Sine"));
      dsp.mux(MOD_SOURCE_SELECT_STEREOSWSLEW_ADDR,0);
  	}
    else if (wave==1) Serial.print(F("Triangle"));
    else if (wave==2) Serial.print(F("Sawtooth"));
    else if (wave==3) Serial.print(F("Square"));
    Serial.println(F(" sweep"));
  }
	  
  uint16_t frequency = pow(10,(log10(f_max)/step_max*step)) + f_min;
       if (wave==0) dsp.sineSource(MOD_STATIC_SINE_ALG0_MASK_ADDR,frequency);
  else if (wave==1) dsp.triangleSource(MOD_TRIANGLE_ALG0_TRI0_ADDR,frequency);
  else if (wave==2) dsp.sawtoothSource(MOD_SAWTOOTH_ALG0_FREQ_ADDR,frequency);
  else if (wave==3) dsp.squareSource(MOD_STATIC_SQUARE_ALG0_MASK_ADDR,frequency);
  
  step += 1;
  if (step>=step_max)
  {
    step = 0;
    wave += 1;
    if (wave>=WAVEFORMS) wave = 0;
  }
  
  delay(25);
}
