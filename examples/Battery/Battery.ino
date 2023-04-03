// please note that when the USB is plugged in it is actively charging the lipo battery which means the battery monitoring
// will report the battery as being fully charged. unplug the USB to see the true voltage of the battery by the RGB color or 
// or by sending the reading via MQTT.

#include <BMS3.h>

BMS3 bms3;

void setup() {
  Serial.begin(115200);

  // Initialize all board peripherals, call this first
  bms3.begin();

  // Brightness is 0-255. We set it to 1/3 brightness here
  bms3.setPixelBrightness(255 / 3);
}

// Gets the battery voltage and shows it using the neopixel LED.
// These values are all approximate, you should do your own testing and
// find values that work for you.
void checkBattery() {
  // gets the battery voltage and stores it in the variable ""battery"
  float battery = bms3.getBatteryVoltage(); 
  // we can print out the battery voltage. 4.2v is full and 3.0v is dead 
  Serial.println(String("Battery: ") + battery);


  //or we can map the voltage to a precentage between 0-100 for easier reading
  int battery_precentage = map(battery,3.0,4.2,0,100);
  Serial.print(String("Battery Precentage: ") + battery_precentage);
  Serial.println("%");
  

  if (bms3.getVbusPresent()) {
    // If USB power is present
    if(battery < 2.0){
      // if no battery detected turn off RGB LED
      bms3.setPixelColor(off);
    }
    if (battery >= 2.0 && battery <= 4.0) {
      bms3.setPixelColor(orange);
      for(int i = 0; i < 100; i++){
      bms3.setPixelBrightness(i);
      delay(10);

      }
    } if(battery > 4.0){
      // almost full
      bms3.setPixelColor(green);
    }
  } else {
    // Else, USB power is not present (running from battery)
    if (battery < 3.1) {
      // Uncomment the following line to sleep when the battery is critically low
      //esp_deep_sleep_start();
    } else if (battery < 3.3) {
      // Below 3.3v - red
      bms3.setPixelColor(red);
    } else if (battery < 3.6) {
      // Below 3.6v (around 50%) - orange
      bms3.setPixelColor(orange);
    } else {
      // Above 3.6v - green
      bms3.setPixelColor(green);
    }
  }
}

// Store the millis of the last battery check
unsigned long lastBatteryCheck = 0;
// Define the battery check interval as one second
#define BATTERY_CHECK_INTERVAL 1000

void loop() {
  if (lastBatteryCheck == 0 || millis() - lastBatteryCheck > BATTERY_CHECK_INTERVAL) {
    checkBattery();
    lastBatteryCheck = millis();
  }
}
