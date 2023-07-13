// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
// Set RTC using an NTP server
// Code contributed to the repo by: occam
#include <WiFi.h>
#include "time.h"
#include "sntp.h"
#include "RTClib.h"
#include <BDL.h>

const char* ssid       = "Your WiFi SSID";
const char* password   = "Your WiFi password";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

// A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
const char* time_zone = "CST6CDT,M3.2.0,M11.1.0"; // Chicago - Central Time
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};    

BDL bdl;
RTC_DS3231 rtc;

//Set the RTC using an ntp server
void setRTC(){
  struct tm timeinfo;

  configTzTime(time_zone, ntpServer1, ntpServer2);
  while(!getLocalTime(&timeinfo)){
    Serial.println("No time available (yet)");
    delay(500);
  }
  Serial.println("Setting time from ntp server");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println();
  rtc.adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
}

void connectWiFi(){
 Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("."); 
  }
  Serial.println(" CONNECTED");
}

void setup () {
  Serial.begin(115200);   
  delay(5000);                   // while(!Serial) doesn't work 
  Serial.println("Booting ....");

  bdl.begin();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
     
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    sntp_servermode_dhcp(1); 
    connectWiFi();
    setRTC();
    WiFi.disconnect();
  }
}

void loop () {
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
    delay(3000);
}
