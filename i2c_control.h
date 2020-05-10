// receive tetris controls over i2c 

#include <Wire.h>

class BinaryInput {
private:
  boolean state;

public:
  BinaryInput() {
    state = false;
  }
  boolean checkInput() {
    if (state) {
      state = false;
      return true;
    }
    else {
      return false;
    }
  }
  void setInput(boolean inState) {
    state = inState;
  }
};

class Controls {
private:  
  BinaryInput left, right, up, down;

public:
  Controls() {
    left = BinaryInput();
    right = BinaryInput();
    up = BinaryInput();
    down = BinaryInput();
  }
  boolean checkLeft() {
    return left.checkInput();
  }
  boolean checkRight() {
    return right.checkInput();
  }
  boolean checkUp() {
    return up.checkInput();
  }
  boolean checkDown() {
    return down.checkInput();
  }
  
  void setLeft(boolean inState){
    left.setInput(inState);
  }
  void setRight(boolean inState){
    right.setInput(inState);
  }
  void setUp(boolean inState){
    up.setInput(inState);
  }
  void setDown(boolean inState){
    down.setInput(inState);
  }
};

Controls controls;

void receiveEvent(int howMany)
{
  while(0 < Wire.available()) // loop through received characters
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    switch(c) {
      case 'l':
        controls.setLeft(true);
        break;
      case 'r':
        controls.setRight(true);
        break;
      case 'u':
        controls.setUp(true);
        break;
      case 'd':
        controls.setDown(true);
        break;
    }
  }
}

void i2c_control_setup()
{
  controls = Controls();
  Wire.setSDA(18);
  Wire.setSCL(19);
  Wire.begin(5);                // join i2c bus with address 
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  Serial.println("serial begin");
}
