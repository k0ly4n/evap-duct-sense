// ThingSpeak_send_LG01 - simple ThingSpeak sender using Atmega328 on LG01 LoRa board with Console
#include <Console.h>
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
  bool write_data(String dataString, const char* file_name, const unsigned short int mode)
  {
    File dataFile = FileSystem.open(file_name, mode);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(dataString);
      dataFile.close();
      return true;
    } else {
      return false;
    }
  }
  
  void uploadData() {
    Console.println("\t\tCalling Curl>");

    Process p;
    p.begin("curl");
    p.addParameter("-v");
    p.addParameter("-k");
    p.addParameter(String(UPLOAD_URL));
    p.addParameter("-d");
    p.addParameter("@"+String(post_data_filename));
    
    p.run();    // Run the process and wait for its termination
    Console.print("\t\t\t Curl ended with " + String(p.exitValue()));
  
    Console.print(" with output: ");
    // If there's output from Linux,
    // send it out the Console:
    while (p.available()>0) 
    {
      char c = p.read();
      Console.write(c);
    }
    Console.println("");
    Console.println("\t\t<Call Finished");
  }


void setup()
{
 
  Bridge.begin(115200);
  Console.begin();
//  while (!Console) ; // Wait for console port to be available
  delay(100);
 
  Console.println("ThingSpeak Test!");
}
 
void loop()
{
  radiopacket.temperature = random(10) + 20;
  radiopacket.pressure = random(100000);
  radiopacket.altitude = random(1000);
  radiopacket.humidity = random(100);
  
  String space = "%20";

  bool b_data_write = true;
  
  
  Console.println("- uploading to ThingSpeak -");
  Console.print("\t\tWill send data: ");

  String full_data_string = "api_key=" + String(API_STRING);
  b_data_write  = write_data(full_data_string, post_data_filename, FILE_WRITE);
  Console.print(full_data_string);

  full_data_string = "&field1=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field2=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field3=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field4=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field5=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field6=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field7=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  full_data_string = "&field8=" + 
                      String(radiopacket.temperature) + space + 
                      String(radiopacket.pressure) + space + 
                      String(radiopacket.altitude) + space + 
                      String(radiopacket.humidity);
  b_data_write &= write_data(full_data_string, post_data_filename, FILE_APPEND);
  Console.print(full_data_string);

  Console.println("");
  
  if (b_data_write) {
    uploadData();
  }
             
  Console.println("---");
  delay(15000);
}
