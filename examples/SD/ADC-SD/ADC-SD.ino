#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <BDL.h>
#include "RTClib.h"

RTC_DS3231 rtc;
BDL bdl;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

String dataMessage;

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
  if(cardType == CARD_MMC) Serial.println("MMC");
  else if(cardType == CARD_SD) Serial.println("SDSC");
  else if(cardType == CARD_SDHC) Serial.println("SDHC");
  else Serial.println("UNKNOWN");

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.print(message);
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  file.print(message);
  file.close();
}

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(100);

  bdl.begin();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  initSDCard();

  File file = SD.open("/test.txt");
  if(!file) {
    Serial.println("Creating file...");
    writeFile(SD, "/test.txt", 
      "Date,Day,Time,Temperature(BF),Battery(V),Pin3(V),Pin4(V),Pin5(V),Pin6(V),Pin7(V),Pin8(V),Pin9(V)\r\n");
  }
  file.close();
}

void loop() {
  DateTime now = rtc.now();
  float temp = (rtc.getTemperature() * 1.8) + 32;
  float battery = bdl.getBatteryVoltage();

  float pin3v = bdl.getADCVoltage(3);
  float pin4v = bdl.getADCVoltage(4);
  float pin5v = bdl.getADCVoltage(5);
  float pin6v = bdl.getADCVoltage(6);
  float pin7v = bdl.getADCVoltage(7);
  float pin8v = bdl.getADCVoltage(8);
  float pin9v = bdl.getADCVoltage(9);

  String dataMessage = String(now.month()) + "/" + String(now.day()) + "/" + String(now.year()) + "," +
                       String(daysOfTheWeek[now.dayOfTheWeek()]) + "," +
                       String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "," +
                       String(temp) + "," + String(battery) + "," +
                       String(pin3v) + "," + String(pin4v) + "," + String(pin5v) + "," +
                       String(pin6v) + "," + String(pin7v) + "," + String(pin8v) + "," + String(pin9v) + "\r\n";

  Serial.print("Saving data: ");
  Serial.println(dataMessage);

  appendFile(SD, "/test.txt", dataMessage.c_str());

  delay(timerDelay);
}
