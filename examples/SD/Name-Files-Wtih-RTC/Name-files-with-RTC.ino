// this example shows how to name the files with the date from the RTC. this can be handy if you want to seperate data you collect by each day 
// instead of collecting all the data in one large file.


#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <BDL.h>
#include "RTClib.h"

RTC_DS3231 rtc;

BDL bdl;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

String dataMessage;

// Initialize SD card
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

// Write to the SD card
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

// Append data to the SD card
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

void setup() {
  Serial.begin(115200);
  while(!Serial){ // need this while loop for serial to work reliably on ESP32-S2/S3 chips. remove it once you're done testing.
    delay(100);  
  }
  bdl.begin();
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

}

void loop() {

  DateTime now = rtc.now();
  float temp = 0; 
  temp = (rtc.getTemperature()*1.8) + 32;
  float battery = bdl.getBatteryVoltage();
  initSDCard();

  String year = String(now.year(), DEC);
  String month = String(now.month(), DEC);
  String day = String(now.day(), DEC);
  String hour = String(now.hour(), DEC);
  String minute = String(now.minute(), DEC);
  String second = String(now.second(), DEC);

  String theCurrentDate = String("/" + month + "-" + day +"-" + year+ ".txt");  // we save the current date to a string so we can use it later to name our files.
 
  
  // If the file doesn't exist
  // Create a file on the SD card and write the labels
  File file = SD.open(theCurrentDate.c_str() );  // here we are checking to see if the current date file already exist or not.
  if(!file) {
    Serial.println("File doesn't exist");// if it doesn't exist then we create a new file with the current date.
    Serial.println("Creating file...");
    writeFile(SD, theCurrentDate.c_str(), "Date, Day, Time, Temperature, Battery \r\n"); // this is the header of the file. so the top of the file will have these labels.
    // and we will add our data for each item under this labels.
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();


    //Concatenate all info separated by commas
    dataMessage =  String(month +"/" + day +"/" + year) + "," + String(daysOfTheWeek[now.dayOfTheWeek()]) + "," + String(hour + ":" + minute + ":" + second ) + "," + String(temp) + "," + String(battery) +"\r\n";
    Serial.print("Saving data: ");
    Serial.println(dataMessage);

    //Append the data to file
    appendFile(SD, theCurrentDate.c_str(), dataMessage.c_str()); // if there is already a file with the current date then we append our new data to that file.
 
  delay(5000);
}
