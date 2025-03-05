
#include <BDL.h>

BDL bdl;

void setup() {
  Serial.begin(115200);

  // Initialize all board peripherals, call this first
  bdl.begin();

  // Brightness is 0-255. We set it to 1/3 brightness here
  bdl.setPixelBrightness(255 / 3);
}

// Gets the battery voltage and shows it using the neopixel LED.
// These values are all approximate, you should do your own testing and
// find values that work for you.
void checkBattery() {
  // gets the battery voltage and stores it in the variable ""battery"
  float battery = bdl.getBatteryVoltage(); 
  // we can print out the battery voltage. 4.2v is full and 3.0v is dead 
  Serial.println(String("Battery: ") + battery);


  //or we can map the voltage to a precentage between 0-100 for easier reading
  int battery_precentage = map(battery*100L,300,420,0,100);
  Serial.print(String("Battery Precentage: ") + battery_precentage);
  Serial.println("%");

  if (bdl.getVbusPresent()) {
    // If USB power is present
    if(battery < 2.0){
      // if no battery detected turn off RGB LED
      bdl.setPixelColor(off);
    }
    if (battery >= 2.0 && battery <= 4.0) {
      bdl.setPixelColor(orange);
      for(int i = 0; i < 100; i++){
      bdl.setPixelBrightness(i);
      delay(10);

      }
    } if(battery > 4.0){
      // almost full
      bdl.setPixelColor(green);
    }
  } else {
    // Else, USB power is not present (running from battery)
    if (battery < 3.1) {
      // Uncomment the following line to sleep when the battery is critically low
      //esp_deep_sleep_start();
    } else if (battery < 3.3) {
      // Below 3.3v - red
      bdl.setPixelColor(red);
    } else if (battery < 3.8) {
      // Below 3.8v (around 50%) - orange
      bdl.setPixelColor(orange);
    } else {
      // Above 3.8v - green
      bdl.setPixelColor(green);
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
