// BME280_LoRa_Receiver_LG01_8ch

#include <FileIO.h>
#include <SPI.h>
#include <RH_RF95.h>

#define LED A2

#define USE_THINGSPEAK
  #define API_STRING "xxxxxxxxxxxxxxxx"
  const char* post_data_filename = "/tmp/data";
  #define UPLOAD_URL "https://api.thingspeak.com/update"
  #define CURL_TIMEOUT 5

#define USE_FILE
  #ifdef USE_FILE
    const char* data_filename = "/mnt/sda1/data/datalog.csv";
  #endif

#define USE_CONSOLE
//#define USE_EXTRA_DEBUG

// LoRA
#define RFM95_RST 9
#define RF95_FREQ 433.0

#ifdef USE_CONSOLE
  #include <Console.h>
#endif
 
RH_RF95 rf95;

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

#ifdef USE_FILE
  String getTimeStamp() {
    String result;
    Process time;
    time.begin("date");
    time.addParameter("+%s");
    time.run();
    
    while(time.available()>0) {
      char c = time.read();
      if(c != '\n')
        result += c;
    }
    
    return result;
  }
#endif

  /* CSV and POST request */
  bool write_data(String dataString, const char* file_name, const unsigned short int mode, const bool b_newLine)
  {
    File dataFile = FileSystem.open(file_name, mode);
  
    // if the file is available, write to it:
    if (dataFile) {
      if (b_newLine) {
        dataFile.println(dataString);        
      } else {
        dataFile.print(dataString);        
      }
      dataFile.close();
      return true;
    } else {
      return false;
    }
  }

#ifdef USE_THINGSPEAK
  void uploadData() {
    Process p;
    p.begin("curl");
    p.addParameter("-k");
    p.addParameter("--connect-timeout");
    p.addParameter(String(CURL_TIMEOUT));    
    p.addParameter(String(UPLOAD_URL));
    p.addParameter("-d");
    p.addParameter("@"+String(post_data_filename));
    p.run();
    
  #ifdef USE_CONSOLE
    Console.print("\tCurl ended with " + String(p.exitValue()));
    Console.print(" with output: ");

    while (p.available()>0) 
    {
      char c = p.read();
      Console.write(c);
    }
    Console.println("");
  #endif
  }

#endif

void setup() 
{
  pinMode(LED, OUTPUT);     
//  pinMode(RFM95_RST, OUTPUT);
//  digitalWrite(RFM95_RST, HIGH);
 
  Bridge.begin(115200);

  #ifdef USE_CONSOLE
    Console.begin();
//  while (!Console) ;
  #endif

  delay(100);

  #ifdef USE_FILE  
    FileSystem.begin();
  #endif

  #ifdef USE_CONSOLE
    Console.println("Arduino LoRa RX Test!");
  #endif

/*  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 */
  while (!rf95.init()) {
    #ifdef USE_CONSOLE
      Console.println("LoRa radio init failed");
    #endif
    while (1);
  }
  #ifdef USE_CONSOLE
    Console.println("LoRa radio init OK!");
  #endif

  if (!rf95.setFrequency(RF95_FREQ)) {
    #ifdef USE_CONSOLE
      Console.println("setFrequency failed");
    #endif
    while (1);
  }
  
  #ifdef USE_CONSOLE
    Console.print("Set Freq to: "); Console.println(RF95_FREQ);
  #endif

  rf95.setTxPower(23, false);
}

void loop()
{
  if (rf95.available())
  {
    if (rf95.recv((uint8_t*)&radiopacket, &datalen))
    {
      digitalWrite(LED, HIGH);
      
      #ifdef USE_CONSOLE
        //RH_RF95::printBuffer("Received: ", radiopacket, datalen);
        Console.print("Count: ");
        Console.println(counter);
      #endif

      if (datalen == sizeof(radiopacket)) {
        
        recv_state = true;

        #ifdef USE_CONSOLE
          Console.println("\tData is valid :)");

          #ifdef USE_EXTRA_DEBUG
            for (uint8_t t=0; t<8; t++) {
              Console.print("BME Data #"); Console.println(t);
    
              Console.print("\tTemperature: ");
              Console.print(radiopacket.bme_datas[t].temperature, 2);
              Console.println(" degrees C");
    
              Console.print("\tPressure: ");
              Console.print(radiopacket.bme_datas[t].pressure, 2);
              Console.println(" Pa");
    
              Console.print("\tAltitude: ");
              Console.print(radiopacket.bme_datas[t].altitude, 2);
              Console.println("m");
          
              Console.print("\t%RH: ");
              Console.print(radiopacket.bme_datas[t].humidity, 2);
              Console.println(" %");
    
              Console.println();
            }
          #endif
        #endif
      }
      
      //Serial.println((char*)radiopacket);
      #ifdef USE_CONSOLE
        Console.print("\tRSSI: ");
      #endif
      last_rssi = rf95.lastRssi();
      #ifdef USE_CONSOLE
        Console.println(last_rssi, DEC);
      #endif
      
      digitalWrite(LED, LOW);

      #ifdef USE_CONSOLE
        Console.println("");
        Console.println("---");
      #endif

/* CSV prepare */
#ifdef USE_FILE
  bool b_csv_data_write = true;

  String timestamp = getTimeStamp();

  #ifdef USE_CONSOLE
    Console.println("- writing file -");
  #endif

  String dataString = timestamp;
         dataString += ";";
         dataString += String(counter);
         dataString += ";";
  b_csv_data_write &= write_data(dataString, data_filename, FILE_APPEND, false);
  #ifdef USE_CONSOLE
    #ifdef USE_EXTRA_DEBUG
      Console.print("\tWill write data: ");
      Console.print(dataString);
    #endif
  #endif

  if (recv_state) {
      for (uint8_t t=0; t<8; t++) {
        dataString  = String(radiopacket.bme_datas[t].temperature) + ";" +
                      String(radiopacket.bme_datas[t].pressure) + ";" +
                      String(radiopacket.bme_datas[t].altitude) + ";" +
                      String(radiopacket.bme_datas[t].humidity) + ";";

        b_csv_data_write &= write_data(dataString, data_filename, FILE_APPEND, false);
        #ifdef USE_CONSOLE
          #ifdef USE_EXTRA_DEBUG
            Console.print(dataString);
          #endif
        #endif
      }
  }
  
  dataString = String(last_rssi); 
  b_csv_data_write &= write_data(dataString, data_filename, FILE_APPEND, true);
  #ifdef USE_CONSOLE
    #ifdef USE_EXTRA_DEBUG
      Console.println(dataString);
    #endif
  #endif
#endif

#ifdef USE_THINGSPEAK
  if (recv_state) {

    String space = "%20";
    bool b_data_write = true;

    #ifdef USE_CONSOLE
      Console.println("- uploading to ThingSpeak -");
    #endif
  
    String full_data_string = "api_key=" + String(API_STRING);
    b_data_write &= write_data(full_data_string, post_data_filename, FILE_WRITE, false);

    #ifdef USE_CONSOLE
      #ifdef USE_EXTRA_DEBUG
        Console.print("\tWill send data: ");
        Console.print(full_data_string);
      #endif
    #endif

    if (recv_state) {
        for (uint8_t t=0; t<8; t++) {
          full_data_string = "&field" + String(t+1) + "=" + 
                              String(radiopacket.bme_datas[t].temperature) + space + 
                              String(radiopacket.bme_datas[t].pressure) + space + 
                              String(radiopacket.bme_datas[t].altitude) + space + 
                              String(radiopacket.bme_datas[t].humidity);
          b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND, false);

          #ifdef USE_CONSOLE
            #ifdef USE_EXTRA_DEBUG
              Console.print(full_data_string);
            #endif
          #endif
        }
    }

    #ifdef USE_CONSOLE
      Console.println("");
    #endif
    
    if (b_data_write) {
      uploadData();
    }
  }
#endif

      #ifdef USE_CONSOLE
        Console.println("===");
      #endif
      counter++;
    }
    else
    {
      #ifdef USE_CONSOLE
        Console.println("Receive failed");
      #endif
      recv_state = false;
    }
  }
}
