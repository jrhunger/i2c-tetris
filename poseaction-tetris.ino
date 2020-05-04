/*------------------------------------------------------------------------------
// Teensy Octo Posenet Tetris
// 
*/
#include "xp16x25.h"
#include "tetris.h"


void setup() {
  xp16x25Setup();
  Serial.begin(115200);
  tetrisSetup();
  
}

void loop() {
  tetrisLoop();
}
