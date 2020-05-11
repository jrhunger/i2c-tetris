/*------------------------------------------------------------------------------
// Teensy Octo Posenet Tetris
// 
*/
#define LOOP_LED 13

#include "xp16x25.h"
#include "tetris.h"

long ledtime;

void setup() {
  pinMode(LOOP_LED,OUTPUT);
  digitalWrite(LOOP_LED,HIGH); 
  xp16x25Setup();
  tetrisSetup();
  ledtime = millis();
}

void loop() {
  if (millis() - ledtime > 500) {
    digitalWrite(LOOP_LED, HIGH);
    ledtime = millis();
  } else {
    if (millis() - ledtime > 250)
    digitalWrite(LOOP_LED, LOW);
  }
  
  tetrisLoop();
}
