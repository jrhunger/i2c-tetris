/* xp16x25.h - functions and variables used to control my homebrew 16x25 
 *  display controlled by Teensy OctoWS2811
 *  x goes left to right
 *  y goes top to bottom
 *  
 *  make sure to set ledPowerPin appropriately
 *  
 *  set things up before using:
 *    xp16x25Setup()
 *  
 *  get pixel index (used for input to draw/scale) from x,y coordinates:
 *    xy16x25(unsigned int x, unsigned int y)
 *  
 *  draw pixel:
 *    drawPixel(unsigned int x, unsigned int y, int color)
 *    drawPixel(unsigned int x, unsigned int y, byte red, byte green, byte blue)
 *  
 *  fill screen with one color:
 *    fillScreen(int color)
 *  
 *  change brightness of pixel:   
 *    scalePixel(int pixel, float scale)
 *    
 *  change brightness of all pixels:
 *    adjustBrightness(float scale)
 *  
*/

#include "digits.h"

// ledPowerPin is configured so when it is set high it will turn
// on the power to the LEDs (it's hooked to the base of an NPN transistor
// with the emitter on ground and the collector on PS_ON of the ATX power supply)
// actually there is a resistor and led in series as well for visual indicator
// of when the LED Power "should" be on.
const int ledPowerPin=1;

// OctoWS2811 used to control the LEDs, info @ https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
#include <OctoWS2811.h>
// setup stuff copied from OctoWS2811 examples:
const int ledsPerStrip = 50;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
// See OctoWS2811 docs for possibilities here
const int config = WS2811_RGB | WS2811_800kHz;

// number of strips we have active
const int activeStrips = 8;
const int numPixels = ledsPerStrip * activeStrips;

// variables, here, hardcoded elsewhere, e.g. xy16x25()
const int gridHeight=25;
const int gridWidth=16;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void xp16x25Setup() {
  pinMode(ledPowerPin, OUTPUT);
  digitalWrite(ledPowerPin, LOW);
  digitalWrite(LOOP_LED, LOW);
  // wait 1 second before turning on the power
  delay(1000);
  digitalWrite(ledPowerPin, HIGH);
  digitalWrite(LOOP_LED, HIGH);
  // then wait 1 second before attempting to control the LEDs
  delay(1000);
  leds.begin();
  leds.show();
}

// rgb2int() return int value of r,g,b
int rgb2int(byte r, byte g, byte b) {
  return 256*256*r + 256*g + b;
}

// xy16x25() turns matrix coordinates into the numeric index used by OctoWS2811
// assuming two strands of 50 arranged as 16x25 grid with each strand going
// up/down/up/down/up.
unsigned int xy16x25(unsigned int x, unsigned int y) {
  // keep bad pixels in the drawing range
  y = abs((24-y) % 25);
  x = 15 - abs(x % 16);
  // which bank of 50 is this?
  int banknum = x / 2;
  int bankx = x % 2;
  // if it's an even row y counts up
  if ((bankx & 1) == 0) {
    return banknum * 50 + bankx * 25 + y;
  } else {
    return banknum * 50 + bankx * 25 + 24 - y;
  }
}

// scale brightness of one color byte by floating ratio
byte scaleColor(byte color, float scale) {
  int newColor = color * scale;
  if (newColor > 255) {
    return 255;
  }
  if (newColor < 0 ) {
    return 0;
  }
  return (byte) newColor;
}

// scale brightness of one pixel by floating ratio
int scalePixel(int pixel, float scale) {
  byte r, g, b;
  b = scaleColor((pixel >> 16) & 0xFF, scale);
  g = scaleColor((pixel >> 8 ) & 0xFF, scale);
  r = scaleColor(pixel & 0xFF, scale);
  return((b<<16)+(g<<8)+r);
}

//draw pixel by sequence with int color
void drawPixel(unsigned int p, int color) {
  leds.setPixel(p,color);
}

//draw pixel by sequence with rgb
void drawPixel(unsigned int p, byte red, byte green, byte blue) {
  leds.setPixel(p, red, green, blue);
}

// draw pixel int color
void drawPixel(unsigned int x, unsigned int y, int color) {
  leds.setPixel(xy16x25(x,y),color);
}

// draw pixel rgb
void drawPixel(unsigned int x, unsigned int y, byte red, byte green, byte blue) {
  leds.setPixel(xy16x25(x,y), red, green, blue);
}

// fill up frame
void fillScreen(int color) {
  for (int i = 0; i<numPixels; i++) {
    leds.setPixel(i,color);
  }
  leds.show();
}

void updateScreen() {
  leds.show();
}

// draw 16x25 pixel saved in GIMP C header format and .h file included above
void drawFrame(const int *frames, int frameNum) {
  for (byte y = 0; y < gridHeight; y++) {
    for (byte x = 0; x < gridWidth; x++) {  
      int loc = x + y*gridWidth;
      leds.setPixel(xy16x25(x,y),frames[frameNum*gridWidth*gridHeight+loc]);
    }
  }
  leds.show();
}

// draw 16x25 pixel saved in GIMP C header format and .h file included above
void drawFrame(const int *frames, int frameNum, float brightScale) {
  for (byte y = 0; y < gridHeight; y++) {
    for (byte x = 0; x < gridWidth; x++) {  
      int loc = x + y*gridWidth;
      leds.setPixel(xy16x25(x,y),scalePixel(frames[frameNum*gridWidth*gridHeight+loc],brightScale));
    }
  }
  leds.show();
}

void adjustBrightness(float scale) {
  int pixel;  
  for (int i = 0; i < numPixels; i++) {
    pixel=leds.getPixel(i);
    leds.setPixel(i, scalePixel(pixel,scale));
  }
  leds.show();
}

void fillArea(int x, int y, int w, int h) {
  int i, j;
  for (i = 0; i<w; i++) {
    for (j = 0; j<h; j++) {
      drawPixel(x+i, y+j, 0);
    }
  }
  leds.show();
}

// drawDigit - x,y is the top-left corner to draw it at
void drawDigit(int digit, int x, int y) {
  int i;
  int j;
  for (i = 0; i < digits_w; i++) {
    for (j = 0; j < digits_h; j++) { 
      if (digits[digit][digits_w*i + j] == 1) {
        drawPixel(x+i, y+j, 200,200,200);
      }
      else {
        drawPixel(x+i, y+j, 0);
      }
    }
  }
  leds.show();
}
