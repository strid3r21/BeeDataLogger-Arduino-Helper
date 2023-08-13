#include <BDL.h> 
BDL bdl; 

void setup() {
  Serial.begin(115200);
  Serial.println("Serial is open for business");
  bdl.begin();
}

void loop() {
  // setup each ADC pin like so. The Bee Data Logger has 7 ADC pins available. 3 thru 9.
  float pin3v = bdl.getADCVoltage(3);
  float pin4v = bdl.getADCVoltage(4);
  float pin5v = bdl.getADCVoltage(5);
  float pin6v = bdl.getADCVoltage(6);
  float pin7v = bdl.getADCVoltage(7);
  float pin8v = bdl.getADCVoltage(8);
  float pin9v = bdl.getADCVoltage(9);

  // print out the ADC voltages. 
  // NOTE: when pins are left floating (ie, not connected to anything) The voltages will be somewhat random.
  // But when pulled HIGH with a voltage between 0-3.3v or LOW to ground it will read correctly.
  Serial.print("Pin 3:");
  Serial.print(pin3v);
  Serial.print(" ");
  Serial.print("Pin 4:");
  Serial.print(pin4v);
  Serial.print(" ");
  Serial.print("Pin 5:");
  Serial.print(pin5v);
  Serial.print(" ");
  Serial.print("Pin 6:");
  Serial.print(pin6v);
  Serial.print(" ");
  Serial.print("Pin 7:");
  Serial.print(pin7v);
  Serial.print(" ");
  Serial.print("Pin 8:");
  Serial.print(pin8v);
  Serial.print(" ");
  Serial.print("Pin 9:");
  Serial.println(pin9v);
  
  delay(100);

}