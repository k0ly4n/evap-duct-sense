// BME280_LoRa_Receiver_8ch
 
#include <SPI.h>
#include <RH_RF95.h>
 
#define RFM95_RST 9
#define RF95_FREQ 433.0
 
RH_RF95 rf95;
 
#define LED 13

// data struct
typedef struct
{
  float   temperature;
  float   pressure;
  float   altitude;
  float   humidity;
} s_bme_data;

typedef struct
{
  s_bme_data bme_datas[8];
} s_radiopacket;

s_bme_data bme_data;
s_radiopacket radiopacket;

uint8_t datalen = sizeof(radiopacket);
int16_t last_rssi;
bool recv_state = false;
uint16_t counter = 0;

//#define USE_LCD

#ifdef USE_LCD
  #include "U8glib.h"
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 

  char *status_text, temp_text[25], s_tmp[15];
#endif

void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("BME280_LoRa_Receiver_8ch started!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  rf95.setTxPower(23, false);

#ifdef USE_LCD
  u8g.setFont(u8g_font_5x7);
  u8g.setFontPosTop();
  status_text = ":)  Receiver started! :)";
#endif
}
 
void loop()
{
#ifdef USE_LCD
  status_text = "...     Receiving     ...";
#endif
//  recv_state = false;

  if (rf95.available())
  {
    if (rf95.recv((uint8_t*)&radiopacket, &datalen))
    {
#ifdef USE_LCD
      status_text = ":)  Receiver got data :) ";
#endif
      recv_state = true;

      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", radiopacket, datalen);
      Serial.print("Count: ");
      Serial.println(counter);

      if (datalen == sizeof(radiopacket)) {
        for (uint8_t t=0; t<8; t++) {
          Serial.print("BME Data #"); Serial.println(t);

          Serial.print("\tTemperature: ");
          Serial.print(radiopacket.bme_datas[t].temperature, 2);
          Serial.println(" degrees C");
    
          Serial.print("\tPressure: ");
          Serial.print(radiopacket.bme_datas[t].pressure, 2);
          Serial.println(" Pa");
    
          Serial.print("\tAltitude: ");
          Serial.print(radiopacket.bme_datas[t].altitude, 2);
          Serial.println("m");
    
          Serial.print("\t%RH: ");
          Serial.print(radiopacket.bme_datas[t].humidity, 2);
          Serial.println(" %");

          Serial.println();
        }
      }
      
      //Serial.println((char*)radiopacket);
      Serial.print("RSSI: ");
      last_rssi = rf95.lastRssi();
      Serial.println(last_rssi, DEC);
      
      digitalWrite(LED, LOW);

      Serial.println("---");

      counter++;
    }
    else
    {
      Serial.println("Receive failed");
      recv_state = false;
    }
  }

#ifdef USE_LCD
    u8g.firstPage();  
    do {
        u8g.drawStr(0, 0, status_text);
        if (recv_state) {
          sprintf(temp_text, "N: %8d   RSSI: %6s", counter, dtostrf(last_rssi, 5, 0, s_tmp));
          u8g.drawStr(0, 15, temp_text);

          sprintf(temp_text, "Temperature: %10s C", dtostrf(radiopacket.temperature, 5, 2, s_tmp));
          u8g.drawStr(0, 25, temp_text);

          sprintf(temp_text, "Pressure: %12s Pa", dtostrf(radiopacket.pressure, 8, 2, s_tmp));
          u8g.drawStr(0, 35, temp_text);

          sprintf(temp_text, "Altitude: %13s m", dtostrf(radiopacket.altitude, 8, 2, s_tmp));
          u8g.drawStr(0, 45, temp_text);

          sprintf(temp_text, "Humidity: %13s %%", dtostrf(radiopacket.humidity, 8, 2, s_tmp));
          u8g.drawStr(0, 55, temp_text);
        }
    } while( u8g.nextPage() );
#endif

}
