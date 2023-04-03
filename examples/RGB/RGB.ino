#include <BMS3.h> //include the bee motion s3 helper library

BMS3 bms3;  //pull the BMS3 class from the helper and name it bms3 so we can use it below;

void setup() {
  
  bms3.begin(); // Initalize the helper library. if you fail to do this the program will not work as intended.

  // Brightness is 0-255. We set it to 1/2 brightness here
  bms3.setPixelBrightness(255 / 2);
}

int color = 0;

void loop() {
  // colorWheel cycles red, orange, ..., back to red at 256
  bms3.setPixelColor(BMS3::colorWheel(color));
  color++;

  // or you can select a specific color like so
  // the predefined colors you can choose from are green, red, blue, pink, purple, yellow, white, aqua, orange and off.

  // bms3.setPixelColor(blue);  
  // uncomment the line above to use it.

  // or by using RGB color values.

  // bms3.setPixelColor(150,0,150).
  // uncomment the line above to use it.
  // find a list of all color RGB values here https://www.rapidtables.com/web/color/RGB_Color.html
  
  // or you can use any color hex code.
  
  // bms3.setPixelColor(0x00FF0000).
  // uncomment the line above to use it.
  // find hex color codes here https://soft.infomir.com/stb/522/Doc/ColorCodes.html

  delay(15);
}