// BME280_LoRa_Receiver_LG01

#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>
 
//#define RFM95_CS 10
//#define RFM95_RST 9
//#define RFM95_INT 2

#define RF95_FREQ 433.0
 
RH_RF95 rf95;
 
#define LED A2

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

//#define USE_LCD
#define USE_FILE
#define USE_THINGSPEAK

#ifdef USE_LCD
  #include "U8glib.h"
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 

  char *status_text, temp_text[25], s_tmp[15];
#endif

#ifdef USE_FILE
  #include <FileIO.h>
  const char* data_filename = "/mnt/sda1/data/datalog.csv";

  // This function return a string with the time stamp 
  // LG01 will call the Linux "date" command and get the time stamp
  /* from Logger_USB_Flash */
  String getTimeStamp() {
    String result;
    Process time;
    time.begin("date");
    time.addParameter("+%s");
    time.run();  // run the command
  
    // read the output of the command
    while(time.available()>0) {
      char c = time.read();
      if(c != '\n')
        result += c;
    }
    
    return result;
  }
  
  /*Function for data writing*/
  bool write_data(String dataString, const char* file_name)
  {
    File dataFile = FileSystem.open(file_name, FILE_APPEND);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      return true;
    } else {
      return false;
    }
  }

#endif

#ifdef USE_THINGSPEAK
  #include <Process.h>
  #define API_STRING "xxxxxxxxxxxxxxxx"
  
  void uploadData(String dataString) {//Upload Data to ThingSpeak
    // form the string for the URL parameter, be careful about the required "
    String upload_url = "https://api.thingspeak.com/update?api_key=";
    upload_url += API_STRING;
    upload_url += "&";
    upload_url += dataString;
    Console.print("\tWill upload to the URL: ");
    Console.print(upload_url);
    Console.println("");
  
    Console.println("\t\tCall Linux Command to Send Data");
    Process p;    // Create a process and call it "p", this process will execute a Linux curl command
    p.begin("curl");
    p.addParameter("-k");
    p.addParameter(upload_url);
    p.run();    // Run the process and wait for its termination
    Console.print("ended with " + String(p.exitValue()));
  
    Console.print("\t\tFeedback from Linux: ");
    // If there's output from Linux,
    // send it out the Console:
    while (p.available()>0) 
    {
      char c = p.read();
      Console.write(c);
    }
    Console.println("");
    Console.println("\t\tCall Finished");
  }

#endif

void setup() 
{
  pinMode(LED, OUTPUT);     
//  pinMode(RFM95_RST, OUTPUT);
//  digitalWrite(RFM95_RST, HIGH);
 
  Bridge.begin(115200);
  Console.begin();
//  while (!Console) ; // Wait for console port to be available
  delay(100);

#ifdef USE_FILE  
  FileSystem.begin();
#endif
  Console.println("BME280_LoRa_Receiver_LG01 started!");
  
  // manual reset
/*  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 */
  while (!rf95.init()) {
    Console.println("LoRa radio init failed");
    while (1);
  }
  Console.println("LoRa radio init OK!");
 
  if (!rf95.setFrequency(RF95_FREQ)) {
    Console.println("setFrequency failed");
    while (1);
  }
  Console.print("Set Freq to: "); Console.println(RF95_FREQ);
 
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
    //uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    //uint8_t len = sizeof(buf);
    
    if (rf95.recv((uint8_t*)&radiopacket, &datalen))
    {
#ifdef USE_LCD
      status_text = ":)  Receiver got data :) ";
#endif
      recv_state = true;

      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", radiopacket, datalen);
      Console.print("Count: ");
      Console.println(counter);

      Console.print("Temperature: ");
      Console.print(radiopacket.temperature, 2);
      Console.println(" degrees C");

      Console.print("Pressure: ");
      Console.print(radiopacket.pressure, 2);
      Console.println(" Pa");

      Console.print("Altitude: ");
      Console.print(radiopacket.altitude, 2);
      Console.println("m");

      Console.print("%RH: ");
      Console.print(radiopacket.humidity, 2);
      Console.println(" %");
      
      //Console.println((char*)radiopacket);
      Console.print("RSSI: ");
      last_rssi = rf95.lastRssi();
      Console.println(last_rssi, DEC);

      digitalWrite(LED, LOW);

#ifdef USE_FILE
  String timestamp = getTimeStamp();
  
  Console.println("- writing file -");
  String dataString = timestamp;
  dataString += ";";
  dataString += String(counter);
  dataString += ";";
  dataString += String(radiopacket.temperature);
  dataString += ";";
  dataString += String(radiopacket.pressure);
  dataString += ";";
  dataString += String(radiopacket.altitude);
  dataString += ";";
  dataString += String(radiopacket.humidity);
  dataString += ";";
  dataString += String(last_rssi);
  
  write_data(dataString, data_filename);
#endif

#ifdef USE_THINGSPEAK
  String space = "%20";
  Console.println("- uploading to ThingSpeak -");
  uploadData("field1=" + 
             String(radiopacket.temperature) + space + 
             String(radiopacket.pressure) + space + 
             String(radiopacket.humidity));
#endif

      Console.println("---");

      counter++;
    }
    else
    {
      Console.println("Receive failed");
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
