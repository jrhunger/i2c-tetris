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

// everything happens by interrupts so the loop just flashes the LED 
// to show the micro is alive
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
