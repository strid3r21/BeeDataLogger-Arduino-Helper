#include <BDL.h>

BDL bdl;

void setup() {
  Serial.begin(115200);

  // Initialize all board peripherals, call this first
  dbl.begin();

  // Brightness is 0-255. We set it to 1/3 brightness here
  dbl.setPixelBrightness(255 / 3);
} 

void loop() {

  bool USB = dbl.getVbusPresent();
    // If USB power is present
    if(USB == true){
      dbl.setPixelColor(green);
    }
    if (USB == false) {
      dbl.setPixelColor(red);
     

      }
      Serial.println(USB);
      delay(50);
   

}