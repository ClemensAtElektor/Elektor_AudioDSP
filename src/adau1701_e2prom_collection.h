/*
 * Purpose: Library of EEPROM HEX files (E2Prom.Hex) created with SigmaStudio 4.7.
 * Board: Elektor Audio DSP FX Processor a.k.a. ESP32 PICO-D4
 * IDE: 1.8.19
 *
 * Usage:
 *
 * By: Clemens Valens
 * Date: 27/06/2024
 */

#ifndef __ADAU1701_E2PROM_COLLECTION_H__
#define __ADAU1701_HEX_FILES_H__


#include <stdint.h>

#define ADAU1701_FIRMWARE_AUDIO_48000_SIZE  (8*68)
extern uint8_t adau1701_firmware_audio_pass_through_48000[ADAU1701_FIRMWARE_AUDIO_48000_SIZE];

#define ADAU1701_FIRMWARE_I2S_48000_SIZE  (8*68)
extern uint8_t adau1701_firmware_i2s_pass_through_48000[ADAU1701_FIRMWARE_I2S_48000_SIZE];

#define ADAU1701_FIRMWARE_I2S_44100_SIZE  (8*68)
extern uint8_t adau1701_firmware_i2s_pass_through_44100[ADAU1701_FIRMWARE_I2S_44100_SIZE];

#define ADAU1701_FIRMWARE_VCO_SIZE  (8*68)
extern uint8_t adau1701_firmware_vco[ADAU1701_FIRMWARE_VCO_SIZE];

// Adapted from E2Prom AD-style HEX file from Elektor project 130232 pitch shifter.
// Potentiometer on MP9 controls pitch.
// Input on ADC1, output on DAC1 i.e. right channel only.
// Sample rate: 48 kHz
#define ADAU1701_FIRMWARE_PITCHSHIFTER_SIZE  (8*80) /* 80 lines of 8 bytes = 640 bytes*/
extern uint8_t adau1701_firmware_pitchshifter[ADAU1701_FIRMWARE_PITCHSHIFTER_SIZE];

#define ADAU1701_FIRMWARE_NOISE_SIZE  (8*68)
extern uint8_t adau1701_firmware_noise[ADAU1701_FIRMWARE_NOISE_SIZE];

#define ADAU1701_FIRMWARE_CHORUS_SIZE  (8*216)
extern uint8_t adau1701_firmware_chorus[ADAU1701_FIRMWARE_CHORUS_SIZE];

#endif /* __ADAU1701_HEX_FILES_H__ */
