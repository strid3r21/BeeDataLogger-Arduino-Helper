#include <BMS3.h>

BMS3 bms3;

void setup() {
  Serial.begin(115200);

  // Initialize all board peripherals, call this first
  bms3.begin();

  // Brightness is 0-255. We set it to 1/3 brightness here
  bms3.setPixelBrightness(255 / 3);
} 

void loop() {

  bool USB = bms3.getVbusPresent();
    // If USB power is present
    if(USB == true){
      bms3.setPixelColor(green);
    }
    if (USB == false) {
      bms3.setPixelColor(red);
     

      }
      Serial.println(USB);
      delay(50);
   

}