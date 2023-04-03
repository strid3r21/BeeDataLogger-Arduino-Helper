#include <BMS3.h> //include the bee motion s3 helper library

BMS3 bms3;  //pull the BMS3 class from the helper and name it bms3 so we can use it below

void setup() {

  Serial.begin(115200);
  
  bms3.begin();  //initalize the helper library. if you fail to do this the program will not work as intended.
  
  bms3.MotionWakeUp(); // This tells the board to wake up when it detects motion.

  //place your code to run here
  Serial.println("IM AWAKE AND READY TO WORK!");
  delay(5000);

// This puts the board into deep sleep and will turn off the 2nd LDO to save maximum battery life.
// the 2nd LDO powers the RGB LED, Stemma Connector and 3V_2 pin on the board
   esp_deep_sleep_start();
  

}
void loop() {
 
// nothing runs down here because the board wakes up, runs the code once in the setup and then goes
// and goes to sleep before it ever gets to the loop.


 


               
}
