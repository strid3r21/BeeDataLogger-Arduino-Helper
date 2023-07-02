// When plugged into wall outlet power via the USB-C port we can detect if power is present.
// which means while plugged into wall power we can send our data over MQTT and if we lose USB power
// we can save the data to the SD card (as long as you have a lipo battery plugged in to power the board)
// once USB power is back we can dump the data stored on the SD card over MQTT.

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "EspMQTTClient.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <BDL.h>

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        //set this to how many seconds you want the esp32 to sleep for between readings when wall power is out

bool con = false;
BDL bdl;
RTC_DS3231 rtc;
String buffer;

EspMQTTClient client(
  "WifiSSID",       // Your WiFi
  "WifiPassword",	// Your WiFi Password
  "192.168.1.100",  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

String dataMessage;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


// Initialize SD card function
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Write to the SD card function
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card function
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// a modified readFile function that will read the data off the SD card and post it via MQTT
void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);
    File file = fs.open(path);
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    }
        
        Serial.println("Read from file: ");
        

            while(file.available()){  // when a file is available on the SD card
              buffer = file.readStringUntil(','); //store the contents in a buffer. we read until each comma to break up the SD entries into 
                                                  // chunks of data we can process.
              String sdDate = buffer;                                   
                                                                        
              Serial.print("Date: ");                                   
              Serial.println(sdDate);                                   
              client.publish("Bee-Data-Logger/data/date", sdDate);      

              buffer = file.readStringUntil(',');                   //
              Serial.print("Day: ");                                //
              String sdDay = buffer;                                // you'll need a block like this for each data point on the sd card.
              Serial.println(sdDay);                                //
              client.publish("Bee-Data-Logger/data/day", sdDay);    //

              buffer = file.readStringUntil(',');
              Serial.print("Time: ");
              String sdTime = buffer;
              Serial.println(sdTime);
              client.publish("Bee-Data-Logger/data/time", sdTime);

              buffer = file.readStringUntil('\r');                      // the last data point in an entry we read until the carriage return.
              Serial.print("Temp: ");
              String sdTemp = buffer;
              Serial.println(sdTemp);
              client.publish("Bee-Data-Logger/data/temp", sdTemp);

              Serial.println();
              client.loop();
              Serial.write(file.read());
              
              client.publish("Bee-Data-Logger/data/lostPower", sdTime); // we can send the last time we lost power via MQTT.
            }

  file.close();
}

void onConnectionEstablished() //once wifi and mqtt server have connected we can start sending data via MQTT
{
  Serial.println("Connected!");
  client.publish("Bee-Data-Logger/data", "");
  delay(1000);
  initSDCard(); 
  readFile(SD, "/backup.txt");  
}
 

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}


void sendMQTT(){
  String temp;
  DateTime now = rtc.now();
  char dt[16];
  char tm[16];
  sprintf(dt, "%02d/%02d/%02d", now.year(),now.month(),now.day());  //you can change the order of these to get a different date format. 
  sprintf(tm, "%02d:%02d:%02d", now.hour(),now.minute(),now.second());
  temp = (rtc.getTemperature()*1.8) + 32; //converting the RTC temp to fahrenheit
  // temp = rtc.getTemperature(); //uncomment to get temp in Celsius.
  String dow =  String(daysOfTheWeek[now.dayOfTheWeek()]) ;

    client.publish("Bee-Data-Logger/data/date", dt);
    client.publish("Bee-Data-Logger/data/day", dow);
    client.publish("Bee-Data-Logger/data/time", tm);
    client.publish("Bee-Data-Logger/data/temp", temp);
    Serial.print("MQTT Sent");
    Serial.println();
    client.loop();

  
}

void backup2SD(){          //this function backups up the data to the SD card incase of USB power outage.
  bdl.setPixelColor(blue);  // this is a status LED for debugging. you can remove this line to save additional power while using battery power.
  float temp = (rtc.getTemperature()*1.8) + 32;
  DateTime now = rtc.now();
  initSDCard(); 
  File file = SD.open("/backup.txt");
        if(!file) {
          Serial.println("File doesn't exist");
          Serial.println("Creating file...");
          writeFile(SD, "/backup.txt","\n");
        }
        else {
          Serial.println("File already exists");  
        }
          file.close();
  
                // this is the data from the high precision RTC. they need to be stored as strings.
                String year = String(now.year(), DEC);
                String month = String(now.month(), DEC);
                String day = String(now.day(), DEC);
                String hour = String(now.hour(), DEC);
                String minute = String(now.minute(), DEC);
                String second = String(now.second(), DEC);

                //Concatenate all info separated by commas.
                dataMessage =  String("" + month +"/" + day +"/" + year) + "," + String(daysOfTheWeek[now.dayOfTheWeek()]) + "," + String(hour + ":" + minute + ":" + second ) + "," + String(temp) +"\r\n";
                Serial.print("Saving data: ");
                Serial.println(dataMessage); 

                //Append the data to file on the SD card.
                appendFile(SD, "/backup.txt", dataMessage.c_str()); //data is saved to backup.txt.
                delay(1000);
    
}


void setup () {
  Serial.begin(115200);

  bdl.begin(); // initialize the Bee Data Logger helper library. this is required.
  bdl.setPixelBrightness(255 / 2); // set the RGB led to half brightness. sometimes even a 3rd brightness is enough.

  print_wakeup_reason(); // we can wake up the esp32 via a timer or by USB power being restored.

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  bool USB = bdl.getVbusPresent(); // on first bootup or after waking up from deepsleep we check to see if the USB power is connected.
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2,1); //we attached the USB detection pin to be a wake up source.
  if(USB == false ){ // if there is no USB power connected then we are running on battery.
        backup2SD(); // since we've lost power we back up our data to the SD card.
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //if USB power is not restored, we still want to wake up periodically and take a reading and save it to the SD card.
        esp_deep_sleep_start(); // go back to sleep to save battery.
      }
  
 
}

void loop () {
  client.loop();
  bool USB = bdl.getVbusPresent(); // if we've made it here, we check again to see if the USB power is connected.
  if(USB == true){ // if USB power is connected.
        
            sendMQTT(); // we to send data over MQTT as long as there is USB power.
            delay(1000);
          }
    
    
    if(USB == false ){ // if suddenly we lose USB power
        backup2SD(); // we save the most recent reading to the SD card.
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //wake up periodically by the timer to log data to the SD card
        esp_deep_sleep_start(); // go to sleep.
      }

}
