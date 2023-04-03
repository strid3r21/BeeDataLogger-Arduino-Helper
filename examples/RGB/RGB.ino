#include <BDL.h> //include the bee motion s3 helper library

BDL dbl;  //pull the dbl class from the helper and name it dbl so we can use it below;

void setup() {
  
  dbl.begin(); // Initalize the helper library. if you fail to do this the program will not work as intended.

  // Brightness is 0-255. We set it to 1/2 brightness here
  dbl.setPixelBrightness(255 / 2);
}

int color = 0;

void loop() {
  // colorWheel cycles red, orange, ..., back to red at 256
  dbl.setPixelColor(dbl::colorWheel(color));
  color++;

  // or you can select a specific color like so
  // the predefined colors you can choose from are green, red, blue, pink, purple, yellow, white, aqua, orange and off.

  // dbl.setPixelColor(blue);  
  // uncomment the line above to use it.

  // or by using RGB color values.

  // dbl.setPixelColor(150,0,150).
  // uncomment the line above to use it.
  // find a list of all color RGB values here https://www.rapidtables.com/web/color/RGB_Color.html
  
  // or you can use any color hex code.
  
  // dbl.setPixelColor(0x00FF0000).
  // uncomment the line above to use it.
  // find hex color codes here https://soft.infomir.com/stb/522/Doc/ColorCodes.html

  delay(15);
}