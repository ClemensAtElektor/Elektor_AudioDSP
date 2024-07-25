/*
 * Purpose: Upload a binary (BIN, *not* HEX) executable to the 
 *          EEPROM of the Audio DSP FX Processor over a TCP/IP 
 *          connection.
 * Board: ESP32-PICO-KIT a.k.a. ESP32 PICO-D4
 * IDE: 1.8.19
 *
 * Usage:
 * - Set JP1 ('DSP clock') to position 'X1' (pins 1 & 2)
 * - Set JP2 to position 'Selfboot' (pins 2 & 3)
 * - Enter network credentials in credentials.h
 * - Enter my_static_ip, my_gateway and my_subnet below
 * - Upload this sketch to the ESP32
 * - Start a terminal program capable of sending binary files 
 *   over a raw TCP/IP connection. Tera Term can do this
 *   https://teratermproject.github.io/
 *   New connection: TCP/IP, Service: Other, check the port (default 22).
 * - Connect to my_static_ip
 * - Send binary E2Prom.bin file. Make sure to send it as *binary* data.
 *   File > Send file... > check Option Binary
 * - Send the command 'write<CR><LF>' where <CR><LF> are new-line termination
 *   characters (Tera Term > Terminal setup > New-line Transmit: CR+LF)
 *   Check Local echo so you can see what you are typing.
 *   This will burn the program into the EEPROM and start the DSP.
 *   (Programming only takes place if the EEPROM contents are different.)
 *
 * By: Clemens Valens
 * Date: 25/07/2024
 */

#include "Elektor_AudioDSP.h"
Elektor_AudioDSP audiodsp = Elektor_AudioDSP();

#include <WiFi.h>

// Wi-Fi network settings, enter your SSID and password in the file credentials.h
#include "credentials.h"
const char* ssid = MY_SSID;
const char* password = MY_PASSWORD;

// Set addresses of your static IP & gateway
IPAddress my_static_ip(192,168,2,184);
IPAddress my_gateway(192,168,2,1);
IPAddress my_subnet(255,255,255,0);

const int port = 22; // Tera Term default port.
WiFiServer server(port);

#define E2PROM_BUFFER_SIZE  (10000)
uint8_t e2prom_buffer[E2PROM_BUFFER_SIZE];
uint16_t e2prom_buffer_index = 0;

void dump_byte(uint8_t ch, uint16_t e2prom_buffer_index)
{
  static uint8_t column = 0;
  if (e2prom_buffer_index==0) column = 0; // Reset hexdump
  Serial.printf("0x%02x, ",ch);
  column += 1;
  if (column>=8)
  {
    column = 0;
    Serial.printf("\n");
  }
}

bool process_data(uint8_t ch)
{
  static uint8_t i = 0;
  static uint8_t command[10];
  static uint8_t escaped = 0;
  
  if (i==0 && ch=='w')
  {
    command[i++] = ch;
  }
  else if (i==1 && ch=='r')
  {
    command[i++] = ch;
  }
  else if (i==2 && ch=='i')
  {
    command[i++] = ch;
  }
  else if (i==3 && ch=='t')
  {
    command[i++] = ch;
  }
  else if (i==4 && ch=='e')
  {
    command[i++] = ch;
  }
  else if (i==5 && ch==0x0d)
  {
    command[i++] = 0;
  }
  else if (i==6 && ch==0x0a)
  {
    command[i] = 0;
    i = 0;
    Serial.printf("\nGot command '%s'\n",command);
    return true;
  }
  else if (i!=0)
  {
    command[i+1] = 0;
    i = 0;
    Serial.printf("\nIgnoring incomplete command '%s'\n",command);
  }

  if (ch==0xff)
  {
    if (escaped==0) escaped = 1;
    else if (escaped==1)
    {
      // For some reasons 0xff characters are doubled (escaped?)
      escaped = 0;
      // Ignore character.
      return false;
    }
  }
  else escaped = 0;
  
  dump_byte(ch,e2prom_buffer_index);
  e2prom_buffer[e2prom_buffer_index] = ch;
  e2prom_buffer_index += 1;
  return false;
}

void setup(void)
{
  Serial.begin(115200); // For debugging, etc.
  Serial.println("Elektor Audio DSP FX Processor");
  Serial.println("Remote EEPROM writer");
  Serial.println();
  
  // Do not release the DSP at the end of begin (third parameter: false).
  // There is no firmware to load into EEPROM yet.
  audiodsp.begin(NULL,0,false);

  // Configure static IP address. Comment out to use DHCP instead.
  if (!WiFi.config(my_static_ip,my_gateway,my_subnet)) 
  {
    Serial.println("WiFi.config failed");
  }
 
  // Connect to Wi-Fi network.
  Serial.printf("Connecting to %s ",ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED) 
  {
    delay(500);
    Serial.printf(".");
  }
  Serial.printf(" ok\n");
  Serial.printf("Send E2Prom.bin file to %s:%d\n",WiFi.localIP().toString().c_str(),port);
  
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
    Serial.println("Connected.");
    audiodsp.led(true); // LED on.

    // Reset E2PROM buffer.
    e2prom_buffer_index = 0;
    
    while (client.connected())
    {
      while (client.available())
      {
        // Read data from the client.
        uint8_t ch = client.read();
        if (process_data(ch)==true)
        {
          // Stop DSP.
          Serial.println("Stopping DSP.");
          audiodsp.dsp_halt();
          // Write EEPROM.
          if (audiodsp.e2prom_write(e2prom_buffer,e2prom_buffer_index)==true)
          {
            // Start DSP.
            Serial.println("Starting DSP.");
            audiodsp.dsp_reset(false);
          }
          // Get ready for new file.
          e2prom_buffer_index = 0;
        }
      }
    }

    // Close the connection.
    client.stop();
    Serial.println("Disconnected.");
  }
  
  audiodsp.led(false); // LED off.
}
