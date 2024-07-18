/*
 * Purpose: Let SigmaStudio upload programs to the ADAU1701 over a TCP/IP connection.
 * Board: ESP32-PICO-KIT
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 away from 'Selfboot' (i.e. *not* Selfboot, pins 1 & 2)
 * - Upload this sketch to ESP32
 * - Wait for LED1 to switch off before trying to connect from SigmaStudio
 *
 * By: Clemens Valens
 * Date: 7/12/2023
 */

//#define SAMPLE_RATE  (48000)
#include "Elektor_AudioDSP.h"
Elektor_AudioDSP audiodsp = Elektor_AudioDSP();

#include <WiFi.h>

// Wi-Fi network settings, enter your SSID and password in the file credentials.h
#include "credentials.h"
const char* ssid = MY_SSID;
const char* password = MY_PASSWORD;

const int port = 8086; // SigmaStudio default port.
WiFiServer server(port);

#define DSP_I2C_ADDRESS  ((ADAU1701_I2C_ADDRESS>>1)&0xfe)
#define EEPROM_I2C_ADDRESS  ((0xA0>>1)&0xfe)

// ADAU1701 memory map
// - parameter: 1024*4 = 4096 (0x0000 to 0x03FF)
// - program:   1024*5 = 5120 (0x0400 to 0x07FF)
// - registers:           102 (0x0800 to 0x0827)
//               total = 9318 bytes (9248 for EEPROM according to datasheet)
//
// Data is sent in blocks of which the size (including the 10-byte header) is set by
// 'Maximum Buffer Size (in bytes)' from SigmaStudio's TCPIPForm dialog box.
// ('Show TCPIP Settings' of 'TCPIP' block on 'Config' tab of 'Hardware Configuration' sheet.)
// Maximum block size is 1500 bytes, minimum is 20.
//
// A typical program upload consists of (block size 1500):
// - DSP Core Control Register block:   12 (   2 data)
// - 4x program:      3*1500 +  660 = 5160 (5120 data)
// - 3x parameters:   2*1498 + 1130 = 4126 (4096 data)
// - DSP Core Control Register block:   34 (  24 data)
// - DSP Core Control Register block:   12 (   2 data)
//                             total: 9344 (9244 data)

typedef enum
{
  kIdle = 0,
  kHeader = 1,
  kData = 2
}
adau1701_state_t;

typedef struct
{
  uint8_t control_code; // 0x9, 0xa or 0xb.
  uint8_t safeload; // is it?
  uint8_t channel; // is it?
  uint16_t packet_size; // header + data.
  uint8_t ic; // IC to write the data to (normally 0x1 for IC1)
  uint16_t data_size; // Number of bytes that follow the header.
  uint16_t address; // Address to write the data to.
}
adau1701_header_t;

// Unpacked structure, do not use ((uint8_t*)&packet_header)[]
adau1701_header_t packet_header;

#define ADAU1701_HEADER_SIZE  (10)
uint8_t header[ADAU1701_HEADER_SIZE];
uint8_t header_index = 0;
adau1701_state_t state = kIdle;
uint16_t data_size = 0;

#define CONTROL_WRITE  (0x09)
#define CONTROL_READ  (0x0a)
#define CONTROL_READ_RESPONSE  (0x0b)

uint16_t i2c_write_count = 0;

void i2c_write_start(uint8_t i2c_address, uint16_t address)
{
  audiodsp.led(true); // LED on.
  Wire.beginTransmission(i2c_address);
  Wire.write((address>>8)&0xff); // Send high address
  Wire.write(address&0xff); // Send low address
  i2c_write_count = 0;
}

uint16_t i2c_write_byte(uint8_t value)
{
  Wire.write(value);
  i2c_write_count += 1;
  return i2c_write_count;
}

uint16_t i2c_write_end(void)
{
  Wire.endTransmission();
  audiodsp.led(false); // LED off.
  return i2c_write_count;
}

bool process_data(uint8_t ch)
{
  bool result = false;
  
  switch (state)
  {
    case kIdle:
      if (ch==CONTROL_WRITE)
      {
        // New write packet, start collecting header.
        state = kHeader;
        header_index = 0;
        header[header_index] = ch;
        header_index += 1;
        Serial.println("Write packet:");
      }
      else if (ch==CONTROL_READ)
      {
        // New read packet.
        Serial.println("Read packet (unhandled).");
      }
      else if (ch==CONTROL_READ_RESPONSE)
      {
        // New read response packet.
        // I don't think we ever receive any of these.
        Serial.println("Read Response packet (unhandled).");
      }
      else
      {
        // Unknown packet.
        Serial.println("Unknown packet (unhandled).");
      }
      break;
      
    case kHeader:
      // Collect header.
      header[header_index] = ch;
      header_index += 1;
      if (header_index>=sizeof(header))
      {
        // Header completed, fill in struct.
        packet_header.control_code = header[0];
        packet_header.safeload = header[1];
        packet_header.channel = header[2];
        packet_header.packet_size = (header[3]<<8) + header[4];
        packet_header.ic = header[5];
        packet_header.data_size = (header[6]<<8) + header[7];
        packet_header.address = (header[8]<<8) + header[9];
        // Enter data collect mode.
        state = kData;
        data_size = packet_header.data_size;
        
        Serial.printf("- Control code: %02x\n",packet_header.control_code);
        Serial.printf("- Safe load: %02x\n",packet_header.safeload);
        Serial.printf("- Channel: %02x\n",packet_header.channel);
        Serial.printf("- Packet size: %d\n",packet_header.packet_size);
        Serial.printf("- IC: %02x\n",packet_header.ic);
        Serial.printf("- Data size: %d\n",packet_header.data_size);
        Serial.printf("- Address: %04x\n",packet_header.address);

        uint8_t i2c_address = packet_header.ic==1? DSP_I2C_ADDRESS : EEPROM_I2C_ADDRESS;
        //uint8_t i2c_address = DSP_I2C_ADDRESS;
        i2c_write_start(i2c_address,packet_header.address);
      }
      break;
      
    case kData:
      // Collect data.
      if (data_size>0)
      {
        i2c_write_byte(ch);
        data_size -= 1;
      }
      // More data?
      if (data_size==0)
      {
        // Packet completed.
        i2c_write_end();
        state = kIdle;
        result = true;
      }
      break;
  }
  return result;
}

void setup(void)
{
  Serial.begin(115200); // For debugging, etc.
  Serial.println("SigmaStudio TCP/IP server");
  Serial.println();
  
  // Do not release the DSP at the end of begin (third parameter: false), 
  // because Wi-Fi must be configured and started first.
  // There is no firmware to load into EEPROM.
  audiodsp.begin(NULL,0,false);

  // Connect to Wi-Fi network.
  Serial.printf("Connecting to %s ",ssid);
  //Serial.println(ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) 
  {
    delay(500);
    Serial.printf(".");
  }
  Serial.printf(" ok\n");
  Serial.printf("SigmaStudio, please connect to %s:%d\n",WiFi.localIP().toString().c_str(),port);
  
  // Start DSP.
  audiodsp.dsp_run();
  
  // Start server.
  server.begin();

  audiodsp.led(false); // LED off.
}

void loop(void)
{
  // Listen for clients.
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("Client connected.");
    
    header_index = 0;
    state = kIdle;
    data_size = 0;
    
    while (client.connected())
    {
      if (client.available())
      {
        // Read data from the client.
        uint8_t ch = client.read();
        if (process_data(ch)==true)
        {
          Serial.println("Packet completed.");
          Serial.println();
        }
      }
    }

    // Close the connection:
    client.stop();
    Serial.println("Client disconnected.");
  }
  
  audiodsp.led(false); // LED off.
}
