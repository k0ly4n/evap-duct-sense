// BME280_LoRa_Receiver
 
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
  
s_bme_data radiopacket;
uint8_t datalen = sizeof(radiopacket);
int16_t last_rssi;
bool recv_state = false;
uint16_t counter = 0;

#define USE_LCD

#ifdef USE_LCD
  #include "U8glib.h"
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 
  #include <Wire.h>
  #define LCD_PIN 3

  char *status_text, temp_text[25], s_tmp[15];
#endif

void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  
  #ifdef USE_LCD
    pinMode(LCD_PIN, OUTPUT);
    Wire.begin();
    digitalWrite(LCD_PIN, HIGH);
  #endif
  
  digitalWrite(RFM95_RST, HIGH);
 
  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println("BME280_LoRa_Receiver started!");
  
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

      Serial.print("Temperature: ");
      Serial.print(radiopacket.temperature, 2);
      Serial.println(" degrees C");

      Serial.print("Pressure: ");
      Serial.print(radiopacket.pressure, 2);
      Serial.println(" Pa");

      Serial.print("Altitude: ");
      Serial.print(radiopacket.altitude, 2);
      Serial.println("m");

      Serial.print("%RH: ");
      Serial.print(radiopacket.humidity, 2);
      Serial.println(" %");
      
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
