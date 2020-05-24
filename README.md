# i2c-tetris
tetris on teensy controlled via i2c
* repo starting point was stub .ino #including tetris.h which was straight lift from electronoobs:
  * https://electronoobs.com/eng_arduino_tut104_code1.php
* changed graphics to use 16x25 OctoWS2811 matrix
  * xp16x25.h from previous project
* changed control to use i2c
* added on-screen score display
* renamed from poseaction-tetris to i2c-tetris
  * no need to name the component for the system that will use it

## i2c commands
i2c settings currently hardcoded in i2c_control.h:i2c_control_setup()
* SDA (18), SDA (19) - available i2c pins on my teensy
* device address 5 (arbitrary, assuming 1-1 bus)

Tetris is simple, so the commands are simple.  Reads characters and accepts the following:
- l - left
- r - right
- u - up (rotate)
- d - down (drop faster)

## Reference:
* teensy i2c: https://www.pjrc.com/teensy/td_libs_Wire.html
* OctoWS2811
  * adaptor: https://www.pjrc.com/store/octo28_adaptor.html
  * library: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
