#include <BDL.h>

BDL bdl;

void setup() {
  Serial.begin(115200);

  // Initialize all board peripherals, call this first
  bdl.begin();

  // Brightness is 0-255. We set it to 1/3 brightness here
  bdl.setPixelBrightness(255 / 3);
} 

void loop() {

  bool USB = bdl.getVbusPresent();
    // Check is usb is connected. make sure to use a lipo battery as well as otherwise when you unplug
    // the usb the board will shut off and wont show you the red led indicator that no USB is detected.
    if(USB == true){
      bdl.setPixelColor(green);
    }
    if (USB == false) {
      bdl.setPixelColor(red);
     

      }
      Serial.println(USB);
      delay(50);
   

}