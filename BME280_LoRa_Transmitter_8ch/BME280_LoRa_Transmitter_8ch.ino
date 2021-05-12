// BME280_LoRa_Transmitter_8ch

#include "Wire.h"
extern "C" { 
#include "utility/twi.h"
}

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_RST 9

#define RF95_FREQ 433.0

RH_RF95 rf95;

#include <Wire.h>

#include <stdint.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define TCAADDR 0x70
#define BMEADDR 0x77

#define TCA_VCC_PIN 3

#define DEEP_SLEEP
#define PAUSE_MS 55000

#ifdef DEEP_SLEEP
  #include <DeepSleepScheduler.h>
#endif

unsigned long etime;
bool radio_ok = true;
unsigned int try_no = 1;

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


Adafruit_BME280 bme_setup(){
    Adafruit_BME280 bme;
    bool status;

    status = bme.begin();  
    if (!status) {
        Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
        //while (1);
    }
    return bme;
}

s_bme_data bme_printValues(Adafruit_BME280 bme) {
    s_bme_data bme_data;

    // get BME280 readings
    float temperature = bme.readTemperature();
    Serial.print(F("Temperature: "));
    Serial.print(temperature, 2);
    Serial.println(F(" degrees C"));
    
    float pressure = bme.readPressure();
    Serial.print(F("Pressure: "));
    Serial.print(pressure, 2);
    Serial.println(F(" Pa"));
    
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print(F("Altitude: "));
    Serial.print(altitude, 2);
    Serial.println(F("m"));
    
    float humidity = bme.readHumidity();
    Serial.print(F("%RH: "));
    Serial.print(humidity, 2);
    Serial.println(F(" %"));

    Serial.println();

    // pack data
    bme_data.temperature = temperature;
    bme_data.pressure = pressure;
    bme_data.altitude = altitude;
    bme_data.humidity = humidity;

    return bme_data;
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() 
{
#ifdef DEEP_SLEEP
  scheduler.schedule(myloop);
#endif
}

void loop()
{
#ifdef DEEP_SLEEP
  scheduler.execute();
}

void myloop()
{
#endif

// <setup>
  etime = millis();  
 
 // LoRa setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  Serial.print(F("T> Started time: "));
  Serial.println(etime);
  delay(100);

  Serial.println(F("BME280_LoRa_Transmitter_8ch started!"));

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println(F("LoRa radio init failed"));
    if (try_no++ > 5) {
      radio_ok &= false;
      break;
    }
  }
  Serial.println(F("LoRa radio init OK!"));

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println(F("setFrequency failed"));
    radio_ok &= false;//while (1);
  }

  if (radio_ok) {
    Serial.print(F("Set Freq to: ")); Serial.println(RF95_FREQ);
  }
  
  if (radio_ok)  {
    rf95.setTxPower(5, false);
  }

 // TCA setup
    // TCA power on
    pinMode(TCA_VCC_PIN, OUTPUT);
    digitalWrite(TCA_VCC_PIN, HIGH);
    Wire.begin();

  Serial.print(F("Init finished: "));
  Serial.println(millis() - etime);
// </setup>



 // BME read
  Adafruit_BME280 bme; // current BME

    Serial.println(F("\nTCAScanner ready!"));
    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print(F("TCA Port #")); Serial.println(t);
 
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
           Serial.print(F("\tFound I2C 0x"));  Serial.println(addr,HEX);
             if (addr == BMEADDR) {
              // BME here
              bme = bme_setup();
              radiopacket.bme_datas[t] = bme_printValues(bme);
             }
        }
      }
    }
    Serial.println(F("\ndone"));
    
  Serial.print(F("T> BMEs data collected: "));
  Serial.println(millis() - etime);

 // LoRa
  // get BME280 readings
  Serial.print(F("T> LoRA started: "));
  Serial.println(millis() - etime);

  if (radio_ok) {
    Serial.println(F("Sending to rf95_server"));
    // Send a message to rf95_server
    
    Serial.println(F("Sending...")); delay(10);
  
    rf95.send((uint8_t *)&radiopacket, sizeof(radiopacket));
  
    Serial.println(F("Waiting for packet to complete...")); delay(10);
    rf95.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
  
    Serial.println(F("Waiting for reply...")); delay(10);
    if (rf95.waitAvailableTimeout(1000))
    { 
      if (rf95.recv(buf, &len))
     {
        Serial.print(F("Got reply: "));
        Serial.println((char*)buf);
        Serial.print(F("RSSI: "));
        Serial.println(rf95.lastRssi(), DEC);    
      }
      else
      {
        Serial.println(F("Receive failed"));
      }
    }
    else
    {
      Serial.println(F("No reply, is there a listener around?"));
    }
  }

  Serial.print(F("T> LoRA ended: "));
  Serial.println(millis() - etime);

  Serial.print(F("T> Loop step finished: "));
  Serial.println(millis() - etime);
  etime = millis();

  // RFM95 to sleep, TCA9548 off
  rf95.sleep();
  Wire.end();
  digitalWrite(TCA_VCC_PIN, LOW);

#ifndef DEEP_SLEEP
  Serial.print(F("T> Will delay for "));
  Serial.print(PAUSE_MS);
  Serial.println(" ms");
  Serial.println(F("\n- - - - - -"));
  delay(PAUSE_MS);
#else
  Serial.print(F("T> Will sleep for "));
  Serial.print(PAUSE_MS);
  Serial.println(" ms");

  Serial.println(F("\n- - - - - -"));
  delay(100);
  scheduler.scheduleDelayed(myloop, PAUSE_MS);
#endif

}


