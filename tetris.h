/*------------------------------------------------------------------------------
// Tetris stolen from: https://electronoobs.com/eng_arduino_tut104_code1.php
//            Original: https://github.com/MarginallyClever/shift-register-tetris
//------------------------------------------------------------------------------

#include <Adafruit_NeoPixel.h>  //Downlaod here: https://electronoobs.com/eng_arduino_Adafruit_NeoPixel.php
#include "EEPROMAnything.h"
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>          //Downlaod here: https://electronoobs.com/eng_arduino_DFMiniMp3.php

        
// size of the LED grid
#define GRID_W              (8)
#define GRID_H              (16)
#define STRAND_LENGTH       (GRID_W*GRID_H)
#define LED_DATA_PIN        (6)
// did you wire your grid in an 'S' instead of a 'Z'?  change this value to 0.
#define BACKWARDS           (0)
// max size of each tetris piece
#define PIECE_W             (4)
#define PIECE_H             (4)
// how many kinds of pieces
#define NUM_PIECE_TYPES     (7)
// joystick options
#define JOYSTICK_DEAD_ZONE  (30)
#define JOYSTICK_PIN        (2)
// gravity options
#define DROP_MINIMUM        (25)  // top speed gravity can reach
#define DROP_ACCELERATION   (20)  // how fast gravity increases
#define INITIAL_MOVE_DELAY  (100)
#define INITIAL_DROP_DELAY  (500)
#define INITIAL_DRAW_DELAY  (30)
//////////////////////////////////////////////////////////////////




// implement a notification class, // its member methods will get called //
class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);   
  }
  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);     
  }
  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code); 
  }
  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);  
  }
};
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);
//////////////////////////////////////////////////////////////////




//Inputs/outputs
#define MAX7219_Data_IN 3
#define MAX7219_Chip_Select  4
#define MAX7219_Clock 5
int button_left = 8;
int button_right = 9;
int button_up = 11;
int button_down = 10;
int button_pause = 7;
int button_start = 12;
int buzzer = 13;
int joystick_x = A0;
int joystick_y = A1;


//Variables
byte adr = 0x08;
byte num = 0x00;
int i = 0;
int top_score = 0;
int score = 0;
int top_score_1 = 0;
int top_score_2 = 0;
int top_score_3 = 0;
int top_score_4 = 0;
int score_1 = 0;
int score_2 = 0;
int score_3 = 0;
int score_4 = 0;
bool Pause = false;
bool pause_onece = false;
bool pause_pressed = false;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;

//Function to shift data on the 7 segment module
void shift(byte send_to_address, byte send_this_data)
{
  digitalWrite(MAX7219_Chip_Select, LOW);
  shiftOut(MAX7219_Data_IN, MAX7219_Clock, MSBFIRST, send_to_address);
  shiftOut(MAX7219_Data_IN, MAX7219_Clock, MSBFIRST, send_this_data);
  digitalWrite(MAX7219_Chip_Select, HIGH);
}
//////////////////////////////////////////////////////////////////




// 1 color drawings of each piece in each rotation.
// Each piece is max 4 wide, 4 tall, and 4 rotations.
const char piece_I[] = {
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  0,0,0,0,

  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,

  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
};

const char piece_L[] = {
  0,0,1,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  0,1,1,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  1,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const char piece_J[] = {
  1,0,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,1,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  0,0,1,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  1,1,0,0,
  0,0,0,0,

};

const char piece_T[] = {
  0,1,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
  
  0,0,0,0,
  1,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,1,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

};

const char piece_S[] = {
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,0,1,0,
  0,0,0,0,

  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,

  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const char piece_Z[] = {
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,0,1,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,
};

const char piece_O[] = {
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
};


// An array of pointers!  
const char *pieces[NUM_PIECE_TYPES] = {
  piece_S,
  piece_Z,
  piece_L,
  piece_J,
  piece_O,
  piece_T,
  piece_I,
};


const long piece_colors[NUM_PIECE_TYPES] = {
  0x009900, // green S
  0xFF0000, // red Z
  0xFF8000, // orange L
  0x000044, // blue J
  0xFFFF00, // yellow O
  0xFF00FF, // purple T
  0x00FFFF,  // cyan I
};


//-------------GLOBALS----------------------------------------------------------
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRAND_LENGTH, LED_DATA_PIN, NEO_RGB + NEO_KHZ800);


// this is how arduino remembers what the button was doing in the past,
// so arduino can tell when it changes.
int old_button=0;
// so arduino can tell when user moves sideways
int old_px = 0;
// so arduino can tell when user tries to turn
int old_i_want_to_turn=0;

// this is how arduino remembers the falling piece.
int piece_id;
int piece_rotation;
int piece_x;
int piece_y;

// the bag from which new pieces are grabbed.
// the bag is only refilled when all pieces are taken.
// this guarantees a maximum of 12 moves between two Is
// or four S & Z in a row.
char piece_sequence[NUM_PIECE_TYPES];
char sequence_i=NUM_PIECE_TYPES;

// this controls how fast the player can move.
long last_move;
long move_delay;  // 100ms = 5 times a second

// this controls when the piece automatically falls.
long last_drop;
long drop_delay;  // 500ms = 2 times a second

long last_draw;
long draw_delay;  // 60 fps

// this is how arduino remembers where pieces are on the grid.
long grid[GRID_W*GRID_H];


//--------------------------------------------------------------------------------
// METHODS
//--------------------------------------------------------------------------------


// I want to turn on point P(x,y), which is X from the left and Y from the top.
// I might also want to hold it on for us microseconds.
void p(int x,int y,long color) {
  int a = (GRID_H-1-y)*GRID_W;
  if( ( y % 2 ) == BACKWARDS ) {  // % is modulus.
    // y%2 is false when y is an even number - rows 0,2,4,6.
    a += x;
  } else {
    // y%2 is true when y is an odd number - rows 1,3,5,7.
    a += GRID_W - 1 - x;
  }
  a%=STRAND_LENGTH;
  strip.setPixelColor(a,color);
}


// grid contains the arduino's memory of the game board, including the piece that is falling.
void draw_grid() {
  int x, y;
  for(y=0;y<GRID_H;++y) {
    for(x=0;x<GRID_W;++x) {
      if( grid[y*GRID_W+x] != 0 ) {
        p(x,y,grid[y*GRID_W+x]);
      } else {
        p(x,y,0);
      }
    }
  }
  strip.show();
}

// choose a new piece from the sequence.
// the sequence is a random list that contains one of each piece.
// that way you're guaranteed an even number of pieces over time,
// tho the order is random.
void choose_new_piece() {
  if( sequence_i >= NUM_PIECE_TYPES ) {
    // list exhausted
    int i,j, k;
    for(i=0;i<NUM_PIECE_TYPES;++i) {
      do {
        // pick a random piece
        j = rand() % NUM_PIECE_TYPES;
        // make sure it isn't already in the sequence.
        for(k=0;k<i;++k) {
          if(piece_sequence[k]==j) break;  // already in sequence
        }
      } while(k<i);
      // not in sequence.  Add it.
      piece_sequence[i] = j;
    }
    // rewind sequence counter
    sequence_i=0;
  }
  
  // get the next piece in the sequence.
  piece_id = piece_sequence[sequence_i++];
  // always start the piece top center.
  piece_y=-4;  // -4 squares off the top of the screen.
  piece_x=3;
  // always start in the same orientation.
  piece_rotation=0;
}


void erase_piece_from_grid() {
  int x, y;
  
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=piece_x+x;
      int ny=piece_y+y;
      if(ny<0 || ny>GRID_H) continue;
      if(nx<0 || nx>GRID_W) continue;
      if(piece[y*PIECE_W+x]==1) {
        grid[ny*GRID_W+nx]=0;  // zero erases the grid location.
      }
    }
  }
}


void add_piece_to_grid() {
  int x, y;
  
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=piece_x+x;
      int ny=piece_y+y;
      if(ny<0 || ny>GRID_H) continue;
      if(nx<0 || nx>GRID_W) continue;
      if(piece[y*PIECE_W+x]==1) {
        grid[ny*GRID_W+nx]=piece_colors[piece_id];  // zero erases the grid location.
      }
    }
  }
}


// Move everything down 1 space, destroying the old row number y in the process.
void delete_row(int y) {
  digitalWrite(buzzer,HIGH);
  delay(35);
  digitalWrite(buzzer,LOW);
  score = score + 10;
  if(score > top_score)
  {
    EEPROM_writeAnything(1, score);
  }
  
  EEPROM_readAnything(1,top_score);
  top_score_1 = top_score - (  ( top_score/10 ) * 10  );
  top_score_2 = ((top_score - (  ( top_score/100 ) * 100  )) - top_score_1)/10;
  top_score_3 = ((top_score - (  ( top_score/1000 ) * 1000  )) - top_score_1 - top_score_2)/100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  
  score_1 = score - (  ( score/10 ) * 10  );
  score_2 = ((score - (  ( score/100 ) * 100  )) - score_1)/10;
  score_3 = ((score - (  ( score/1000 ) * 1000  )) - score_1 - score_2)/100;
  score_4 = (score - score_1 - score_2 - score_3) / 1000;
  
  
  shift(0x08, score_4); //digit 7 (leftmost digit) data
  shift(0x07, score_3);
  shift(0x06, score_2);
  shift(0x05, score_1);
  shift(0x04, top_score_4);
  shift(0x03, top_score_3);
  shift(0x02, top_score_2);
  shift(0x01, top_score_1); //digit 0 (rightmost digit) data

  
  int x;
  for(;y>0;--y) {
    for(x=0;x<GRID_W;++x) {
      grid[y*GRID_W+x] = grid[(y-1)*GRID_W+x];
    }
  }
  // everything moved down 1, so the top row must be empty or the game would be over.
  for(x=0;x<GRID_W;++x) {
    grid[x]=0;
  }
}


void fall_faster() { 
  if(drop_delay > DROP_MINIMUM) drop_delay -= DROP_ACCELERATION;
}


void remove_full_rows() {
  int x, y, c;
  char row_removed=0;
  
  for(y=0;y<GRID_H;++y) {
    // count the full spaces in this row
    c = 0;
    for(x=0;x<GRID_W;++x) {
      if( grid[y*GRID_W+x] > 0 ) c++;
    }
    if(c==GRID_W) {
      // row full!
      delete_row(y);
      fall_faster();
    }
  }  
}


void try_to_move_piece_sideways() {
  // what does the joystick angle say
  int dx = map(analogRead(joystick_x),0,1023,512,-512);
  
  int new_px = 0;
  // is the joystick really being pushed?
  if(dx> JOYSTICK_DEAD_ZONE) {
    new_px=-1;
  }
  if(dx<-JOYSTICK_DEAD_ZONE) {
    new_px=1;
  }
  
  if(!digitalRead(button_left))
  {
    new_px=-1;
  }

  if(!digitalRead(button_right))
  {
    new_px=1;
  }
  

  if(new_px!=old_px && piece_can_fit(piece_x+new_px,piece_y,piece_rotation)==1) {
    piece_x+=new_px;
  }



  
  old_px = new_px;
}


void try_to_rotate_piece() {
  int i_want_to_turn=0;
  
  // what does the joystick button say
  int new_button = digitalRead(button_up);
  // if the button state has just changed AND it is being let go,
  if( new_button > 0 && old_button != new_button ) {
    i_want_to_turn=1;
  }
  old_button=new_button;
  
  // up on joystick to rotate
  int dy = map(analogRead(joystick_y),0,1023,512,-512);
  if(dy>JOYSTICK_DEAD_ZONE) i_want_to_turn=1;
  
  if(i_want_to_turn==1 && i_want_to_turn != old_i_want_to_turn) {
    // figure out what it will look like at that new angle
    int new_pr = ( piece_rotation + 1 ) % 4;
    // if it can fit at that new angle (doesn't bump anything)
    if(piece_can_fit(piece_x,piece_y,new_pr)) {
      // then make the turn.
      piece_rotation = new_pr;
    } else {
      // wall kick
      if(piece_can_fit(piece_x-1,piece_y,new_pr)) {
        piece_x = piece_x-1;
        piece_rotation = new_pr;
      } else if(piece_can_fit(piece_x+1,piece_y,new_pr)) {
        piece_x = piece_x+1;
        piece_rotation = new_pr;
      } 
    }
  }
  old_i_want_to_turn = i_want_to_turn;
}


// can the piece fit in this new location?
int piece_can_fit(int px,int py,int pr) {
  if( piece_off_edge(px,py,pr) ) return 0;
  if( piece_hits_rubble(px,py,pr) ) return 0;
  return 1;
}


int piece_off_edge(int px,int py,int pr) {
  int x,y;
  const char *piece = pieces[piece_id] + (pr * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {
      int nx=px+x;
      int ny=py+y;
      if(ny<0) continue;  // off top, don't care
      if(piece[y*PIECE_W+x]>0) {
        if(nx<0) return 1;  // yes: off left side
        if(nx>=GRID_W ) return 1;  // yes: off right side
      }
    }
  }
  
  return 0;  // inside limits
}


int piece_hits_rubble(int px,int py,int pr) {
  int x,y;
  const char *piece = pieces[piece_id] + (pr * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {    
    int ny=py+y;
    if(ny<0) continue;
    for(x=0;x<PIECE_W;++x) {
      int nx=px+x;
      if(piece[y*PIECE_W+x]>0) {
        if(ny>=GRID_H ) return 1;  // yes: goes off bottom of grid      
        if(grid[ny*GRID_W+nx]!=0 ) return 1;  // yes: grid already full in this space
      }
    }
  }
  
  return 0;  // doesn't hit
}



void draw_restart()
{
  for(int i=0;i<STRAND_LENGTH;i++)
   {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0,0,0)); // Moderately bright green color.  
   }

   /*strip.setPixelColor(12, strip.Color(150,150,150)); 
   strip.setPixelColor(19, strip.Color(150,150,150)); 
   strip.setPixelColor(28, strip.Color(150,150,150)); 
   strip.setPixelColor(34, strip.Color(150,150,150)); 
   strip.setPixelColor(45, strip.Color(150,150,150)); 
   strip.setPixelColor(50, strip.Color(150,150,150)); 
   strip.setPixelColor(51, strip.Color(150,150,150)); 
   strip.setPixelColor(52, strip.Color(150,150,150)); 
   strip.setPixelColor(53, strip.Color(150,150,150)); */
   strip.setPixelColor(55, strip.Color(150,150,150)); 
   digitalWrite(buzzer,HIGH);
   delay(10);
   digitalWrite(buzzer,LOW); 
   strip.setPixelColor(74, strip.Color(150,150,150)); 
   strip.setPixelColor(77, strip.Color(150,150,150));
   strip.setPixelColor(83, strip.Color(150,150,150));  
   strip.setPixelColor(85, strip.Color(150,150,150)); 
   strip.setPixelColor(90, strip.Color(150,150,150)); 
   strip.setPixelColor(91, strip.Color(150,150,150)); 
   strip.setPixelColor(92, strip.Color(150,150,150)); 
   strip.setPixelColor(93, strip.Color(150,150,150)); 
   strip.setPixelColor(98, strip.Color(150,150,150)); 
   strip.setPixelColor(101, strip.Color(150,150,150)); 
   strip.setPixelColor(106, strip.Color(150,150,150)); 
   strip.setPixelColor(107, strip.Color(150,150,150)); 
   strip.setPixelColor(108, strip.Color(150,150,150)); 
   strip.setPixelColor(109, strip.Color(150,150,150)); 
   strip.show(); // This sends the updated pixel color to the hardware.
   /*if(!pause_onece)
   {
    mp3.playMp3FolderTrack(2);  
    pause_onece = true;
    delay(1000);
   }*/
}





void all_white()
{
  for(int i=0;i<STRAND_LENGTH;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(120,120,120)); // Moderately bright green color.
    strip.show(); // This sends the updated pixel color to the hardware.
    delay(3); // Delay for a period of time (in milliseconds).
  }
   
}


void game_over() {
  score = 0;
  mp3.playMp3FolderTrack(4);  // engine start + submarine sound
  game_over_loop_leds();
  delay(1000);
  int x,y;

  long over_time = millis();
  draw_restart();
  currentMillis = millis();
  previousMillis = currentMillis;
  int led_number = 1;
  while(millis() - over_time < 8000) {
    currentMillis = millis();
    if(currentMillis - previousMillis >= 1000){
      previousMillis += 1000;
      digitalWrite(buzzer,HIGH);
      delay(10);
      digitalWrite(buzzer,LOW);    
      strip.setPixelColor(55-led_number, strip.Color(150,150,150));  
      strip.show();
      led_number += 1; 
    }    
   
    
    // click the button?
    if(!digitalRead(button_start)) {
      // restart!
      mp3.playMp3FolderTrack(3);  // engine start + submarine sound
      all_white();
      delay(400);
      break;
    }
  }
  mp3.playMp3FolderTrack(3);  // engine start + submarine sound
  all_white();
  setup();
  return;
}


void game_over_loop_leds()
{
  for(int i=0;i<STRAND_LENGTH;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0,150,0)); // Moderately bright green color.

    strip.show(); // This sends the updated pixel color to the hardware.

    delay(10); // Delay for a period of time (in milliseconds).

  }
  
}


void try_to_drop_piece() {
  erase_piece_from_grid();  
  if(piece_can_fit(piece_x,piece_y+1,piece_rotation)) {
    piece_y++;  // move piece down
    add_piece_to_grid();
  } else {
    // hit something!
    // put it back
    add_piece_to_grid();
    remove_full_rows();
    if(game_is_over()==1) {
      game_over();
    }
    // game isn't over, choose a new piece
    choose_new_piece();
  }
}


void try_to_drop_faster() {
  int y = map(analogRead(joystick_y),0,1023,512,-512);

  if(!digitalRead(button_down))
  {
    try_to_drop_piece();
  }
  
  if(y<-JOYSTICK_DEAD_ZONE) {
    // player is holding joystick down, drop a little faster.
    try_to_drop_piece();
  }
}


void react_to_player() {
  erase_piece_from_grid();
  try_to_move_piece_sideways();
  try_to_rotate_piece();
  add_piece_to_grid();
  
  try_to_drop_faster();
}


// can the piece fit in this new location
int game_is_over() {
  int x,y;
  const char *piece = pieces[piece_id] + (piece_rotation * PIECE_H * PIECE_W);
  
  for(y=0;y<PIECE_H;++y) {
    for(x=0;x<PIECE_W;++x) {      
      int ny=piece_y+y;
      int nx=piece_x+x;
      if(piece[y*PIECE_W+x]>0) {
        if(ny<0) return 1;  // yes: off the top!
      }
    }
  }
  
  return 0;  // not over yet...
}


// called once when arduino reboots
void setup() {
  Serial.begin(115200);  
  mp3.begin();
  uint16_t volume = mp3.getVolume();
  mp3.setVolume(30);  
  uint16_t count = mp3.getTotalTrackCount();
  delay(1000);
  mp3.playMp3FolderTrack(1);  // engine start + submarine sound
  //EEPROM_writeAnything(1, 000);
  
  pinMode(MAX7219_Data_IN, OUTPUT);
  pinMode(MAX7219_Chip_Select, OUTPUT);
  pinMode(MAX7219_Clock, OUTPUT);
  digitalWrite(MAX7219_Clock, HIGH);

  pinMode(button_pause,INPUT_PULLUP);
  pinMode(button_start,INPUT_PULLUP);
  pinMode(button_left,INPUT_PULLUP);
  pinMode(button_right,INPUT_PULLUP);
  pinMode(button_up,INPUT_PULLUP);
  pinMode(button_down,INPUT_PULLUP);

  pinMode(buzzer,OUTPUT);
  delay(1);

  //Setup
  shift(0x0f, 0x00); //display test register - test mode off
  shift(0x0c, 0x01); //shutdown register - normal operation
  shift(0x0b, 0x07); //scan limit register - display digits 0 thru 7
  shift(0x0a, 0x0f); //intensity register - max brightness
  shift(0x09, 0xff); //decode mode register - CodeB decode all digits
  
  EEPROM_readAnything(1,top_score);

  Serial.println(top_score);
  
  top_score_1 = top_score - (  ( top_score/10 ) * 10  );
  top_score_2 = ((top_score - (  ( top_score/100 ) * 100  )) - top_score_1)/10;
  top_score_3 = ((top_score - (  ( top_score/1000 ) * 1000  )) - top_score_1 - top_score_2)/100;
  top_score_4 = (top_score - top_score_1 - top_score_2 - top_score_3) / 1000;
  
  
  shift(0x08, 0x0f); //digit 7 (leftmost digit) data
  shift(0x07, 0x0f);
  shift(0x06, 0x0f);
  shift(0x05, 0x0f);
  shift(0x04, top_score_4);
  shift(0x03, top_score_3);
  shift(0x02, top_score_2);
  shift(0x01, top_score_1); //digit 0 (rightmost digit) data


  
  int i;
  // setup the LEDs
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // set up joystick button
  pinMode(JOYSTICK_PIN,INPUT);
  digitalWrite(JOYSTICK_PIN,HIGH);
  
  // make sure arduino knows the grid is empty.
  for(i=0;i<GRID_W*GRID_H;++i) {
    grid[i]=0;
  }
  
  // make the game a bit more random - pull a number from space and use it to 'seed' a crop of random numbers.
  randomSeed(analogRead(joystick_y)+analogRead(2)+analogRead(3));
  
  // get ready to start the game.
  choose_new_piece();
  
  move_delay=INITIAL_MOVE_DELAY;
  drop_delay=INITIAL_DROP_DELAY;
  draw_delay=INITIAL_DRAW_DELAY;

  // start the game clock after everything else is ready.
  last_draw = last_drop = last_move = millis();
}




//Void used for sound pause
void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    // calling mp3.loop() periodically allows for notifications 
    // to be handled without interrupts
    mp3.loop(); 
    delay(1);
  }
}

void draw_pause()
{  
   for(int i=0;i<STRAND_LENGTH;i++)
   {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0,0,0)); // Moderately bright green color.  
   }
   
   strip.setPixelColor(53, strip.Color(150,150,150)); 
   strip.setPixelColor(58, strip.Color(150,150,150)); 
   strip.setPixelColor(66, strip.Color(150,150,150)); 
   strip.setPixelColor(67, strip.Color(150,150,150)); 
   strip.setPixelColor(68, strip.Color(150,150,150)); 
   strip.setPixelColor(69, strip.Color(150,150,150)); 
   strip.setPixelColor(74, strip.Color(150,150,150)); 
   strip.setPixelColor(77, strip.Color(150,150,150)); 
   strip.setPixelColor(82, strip.Color(150,150,150)); 
   strip.setPixelColor(83, strip.Color(150,150,150)); 
   strip.setPixelColor(84, strip.Color(150,150,150)); 
   strip.setPixelColor(85, strip.Color(150,150,150)); 
   strip.show(); // This sends the updated pixel color to the hardware.
   if(!pause_onece)
   {
    mp3.playMp3FolderTrack(2);  // engine start + submarine sound
    pause_onece = true;
    delay(1000);
   }
}






// called over and over after setup()
void loop() {
  long t = millis();

  if(!Pause)
  {
    if(!digitalRead(button_pause) && !pause_pressed)
    {
      Pause = !Pause;
      pause_pressed = true;
      pause_onece = false;
    }
    if(digitalRead(button_pause) && pause_pressed)
    {      
      pause_pressed = false;
    }
    
    // the game plays at one speed,
    if(t - last_move > move_delay ) {
      last_move = t;
      react_to_player();
    }
    
    // ...and drops the falling block at a different speed.
    if(t - last_drop > drop_delay ) {
      last_drop = t;
      try_to_drop_piece();
    }
    
    // when it isn't doing those two things, it's redrawing the grid.
    if(t - last_draw > draw_delay ) {
      last_draw = t;
      draw_grid();
    }
  }//end of !pause

  else
  {
    if(!digitalRead(button_pause) && !pause_pressed)
    {
      Pause = !Pause;
      pause_pressed = true;
      mp3.playMp3FolderTrack(1);  // engine start + submarine sound
    }
    if(digitalRead(button_pause) && pause_pressed)
    {      
      pause_pressed = false;
    }
    draw_pause();
    delay(1);
  }
}



/** Thank you MarginallyClever
* This file is part of LED8x16tetris.*
* LED8x16tetris is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* LED8x16tetris is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with LED8x16tetris. If not, see <http://www.gnu.org/licenses/>.
*/