// Uno_ThingSpeak_send_emu - simple ThingSpeak sender using Atmega328 on LG01 LoRa board
#include <Process.h>
#include <FileIO.h>

#define API_STRING "xxxxxxxxxxxxxxxx"
const char* post_data_filename = "/tmp/data";
#define UPLOAD_URL "https://api.thingspeak.com/update"


// data struct
typedef struct
{
  float   temperature;
  float   pressure;
  float   altitude;
  float   humidity;
} s_bme_data;
  
s_bme_data radiopacket;

  /*Function for data writing*/
  bool write_post_data(const char* file_name, String dataString)
  {
    File dataFile = FileSystem.open(file_name, FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      return true;
    } else {
      return false;
    }
  }
  
  void uploadData() {
    Serial.println("\t\tCalling Curl>");

    Process p;
    p.begin("curl");
    p.addParameter("-v");
    p.addParameter("-k");
    p.addParameter(String(UPLOAD_URL));
    p.addParameter("-d");
    p.addParameter("@"+String(post_data_filename));
    
    p.run();    // Run the process and wait for its termination
    Serial.print("\t\t\t Curl ended with " + String(p.exitValue()));
  
    Serial.print(" with output: ");
    // If there's output from Linux,
    // send it out the Console:
    while (p.available()>0) 
    {
      char c = p.read();
      Serial.write(c);
    }
    Serial.println("");
    Serial.println("\t\t<Call Finished");
  }


void setup()
{
  Serial.begin(9600);
 
  Serial.println("ThingSpeak Test!");
}
 
void loop()
{
  radiopacket.temperature = random(10) + 20;
  radiopacket.pressure = random(100000);
  radiopacket.altitude = random(1000);
  radiopacket.humidity = random(100);
  
  String space = "%20";
  
  
  Serial.println("- uploading to ThingSpeak -");

  String full_data_string[9] =  {"api_key=" + String(API_STRING) ,//
                                 "&field1=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field2=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field3=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field4=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field5=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field6=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field7=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity) ,//
                                 "&field8=" + String(radiopacket.temperature) + space + 
                                              String(radiopacket.pressure) + space + 
                                              String(radiopacket.altitude) + space + 
                                              String(radiopacket.humidity)};

  Serial.print("  Will send data: ");
  Serial.print(full_data_string[0]+full_data_string[1]+full_data_string[2]+full_data_string[3]+full_data_string[4]+full_data_string[5]+full_data_string[6]+full_data_string[7]+full_data_string[8]);
  Serial.println("");
             
  Serial.println("---");
  delay(15000);
}
