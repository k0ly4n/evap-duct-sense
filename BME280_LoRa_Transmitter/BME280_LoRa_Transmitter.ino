// BME280_LoRa_Transmitter

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_RST 9

#define RF95_FREQ 433.0

RH_RF95 rf95;

#include <Wire.h>
#include <stdint.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

#define SEALEVELPRESSURE_HPA (1013.25)
bool status;

// data struct
typedef struct
{
  float   temperature;
  float   pressure;
  float   altitude;
  float   humidity;
} s_bme_data;
  
s_bme_data radiopacket;

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("BME280_LoRa_Transmitter started!");

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
  rf95.setTxPower(5, false);

  // BME280
  status = bme.begin();
  if (!status) {
      Serial.println("E: could not find a valid BME280 sensor, check wiring!");
  } else {
      Serial.print("I: Starting BME280... result of .begin(): 0x");
      Serial.println(status, HEX);
  }
}

void loop()
{
  // get BME280 readings
  float temperature = bme.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" degrees C");

  float pressure = bme.readPressure();
  Serial.print("Pressure: ");
  Serial.print(pressure, 2);
  Serial.println(" Pa");

  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print("Altitude: ");
  Serial.print(altitude, 2);
  Serial.println("m");

  float humidity = bme.readHumidity();
  Serial.print("%RH: ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  // pack data
  radiopacket.temperature = temperature;
  radiopacket.pressure = pressure;
  radiopacket.altitude = altitude;
  radiopacket.humidity = humidity;
 
  Serial.println("Sending "); 
    Serial.print((uint8_t) temperature, HEX);
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)&radiopacket, sizeof(radiopacket));

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  delay(3000);
}
