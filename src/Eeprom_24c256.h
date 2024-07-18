/*
 * Purpose: EEPROM class
 *
 * By: Clemens Valens
 * Date: 01/07/2024
 *
 * This is started out as a clone of https://github.com/polohpi/AT24C256 but got morphed to my needs.
 */

#ifndef __EEPROM_24C256_H__
#define __EEPROM_24C256_H__


#define EEPROM_I2C_ADDRESS  (0x50)

class Eeprom_24c256
{
private:
  uint8_t _i2c_address;
  
  void write_address(uint16_t address) 
  {
    Wire.beginTransmission(_i2c_address);
    Wire.write((uint8_t)(address>>8));   // MSB
    Wire.write((uint8_t)(address&0xff)); // LSB
  }
  
public:

  Eeprom_24c256(void) : 
    _i2c_address(EEPROM_I2C_ADDRESS)
  {
  }

  void i2c_address_set(uint8_t i2c_address)
  {
    _i2c_address = i2c_address;
  }

  uint8_t i2c_address_get(void) const
  {
    return _i2c_address;
  }
  
  bool is_available(void)
  {
    Wire.beginTransmission(_i2c_address);
    if (Wire.endTransmission()==0) return true;
    return false;
  }
  
  void write(uint8_t val, uint16_t address) 
  {
    write_address(address);
    Wire.write(val);
    Wire.endTransmission();
    delay(5);
  }

  void write(uint8_t *p_data, size_t data_size)
  {
    for (size_t i=0; i<data_size; i++)
    {
      write(p_data[i],i);
    }
  }

  uint8_t read(uint16_t address) 
  {
    write_address(address);
    Wire.endTransmission();  
    Wire.requestFrom((uint8_t)_i2c_address,(uint8_t)1);
    return Wire.read();
  }
  
  void read(uint8_t *p_dst, size_t data_size)
  {
    for (size_t i=0; i<data_size; i++)
    {
      p_dst[i] = read(i);
    }
  }
  
  bool compare(uint8_t *p_data, size_t data_size)
  {
    for (size_t i=0; i<data_size; i++)
    {
      if (p_data[i]!=read(i)) return false;
    }
	return true;
  }
  
};


#endif // __EEPROM_24C256_H__