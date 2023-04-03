#include <BDL.h> //include the bee motion s3 helper library

BDL dbl;  //pull the dbl class from the helper and name it dbl so we can use it below

void setup() {

  Serial.begin(115200);
  
  dbl.begin();  //initalize the helper library. if you fail to do this the program will not work as intended.
  
  // the 2nd LDO (2nd voltage regulator) turns on automatically when the helper program is used. 
  // if you want to turn it off use the code below.
  // the 2nd LDO powers the RGB LED, the stemma connector and pin 3V_2 on the board.

  delay(10000); //delay for 10secs so you can monitor the 3V_2 pin on the board to see it turn off the LDO after 10 seconds

  dbl.setLDO2Power(false);  // this turns off the 2nd LDO.

}
void loop() {
 

 


               
}
