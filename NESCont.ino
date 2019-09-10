/*
================================================================================

    File........... NES Controller Test Code
    Purpose........ To demonstrate how to interface to an NES controller
    Author......... Joseph Corleto -> forked by Jesse Robertson
    E-mail......... corleto.joseph @ gmail.com -> jesse@jrobertson.dev
    Started........ 04/13/2016 -> 3/01/2019
    Finished....... 04/14/2016 -> 3/03/2019
    Updated........ --/--/----
 
================================================================================
   Notes
================================================================================
- The NES controller contains one 8-bit 4021 shift register inside. 

- This register takes parallel inputs and converts them into a serial output.

- This code first latches the data and then shifts in the first bit on the data line. 
  Then it clocks and shifts in on the data line until all bits are received.
  
- What is debugged are the button states of the NES controller.

- A logical "1" means the button is not pressed. A logical "0" means the button is
  pressed.
  
- This code shifts the first bit of data into the LSB.

- The order of shifting for the buttons is shown in the table below:

        Bit# | Button   
        --------------
          0  |   A  
        --------------
          1  |   B  
        --------------
          2  | Select   
        --------------
                      3  | Start  
        --------------
          4  |   Up  
        --------------
          5  |  Down  
        --------------
          6  |  Left   
        --------------
          7  | Right   
        --------------
        
- The NES controller pinout is shown below (looking into controllers
  connector end):
    __________
   /      |
  /       O 1 | 1 - Ground
        |           | 2 - Clock
  | 7 O   O 2 |   3 - Latch
  |           | 4 - Data Out
  | 6 O   O 3 | 5 - No Connection
  |           |   6 - No Connection
  | 5 O   O 4 |   7 - +5V
  |___________|

- Please visit http://www.allaboutcircuits.com to search for complete article!

================================================================================
  Updates
================================================================================
*/

//===============================================================================
//  Libraries
//===============================================================================
#include <LiquidCrystal.h>
//===============================================================================
//  Constants
//===============================================================================
// Here we have a bunch of constants that will become clearer when we look at the
// readNesController() function. Basically, we will use these contents to clear
// a bit. These are chosen according to the table above.
const int A_BUTTON         = 0;
const int B_BUTTON         = 1;
const int SELECT_BUTTON    = 2;
const int START_BUTTON     = 3;
const int UP_BUTTON        = 4;
const int DOWN_BUTTON      = 5;
const int LEFT_BUTTON      = 6;
const int RIGHT_BUTTON     = 7;

//===============================================================================
//  Variables
//===============================================================================
byte nesRegister  = 0;    // We will use this to hold current button states
LiquidCrystal lcd(1, 9, 5, 6, 7, 8);
bool isJump = false;
bool gameOver = false;

// Character animation frames
byte character[8] = {
  B01110,
  B01110,
  B01110,
  B00100,
  B11111,
  B00100,
  B01010,
  B10001,
};

byte charjump1_head[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B01110,
  B01110,
};

byte charjump1_body[8] = {
  B00100,
  B11111,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,
  B00000,
};

byte charjump2_upper[8] = {
  B00000,
  B00000,
  B01110,
  B01110,
  B01110,
  B00100,
  B11111,
  B00100,
};

byte charjump2_lower[8] = {
  B01010,
  B10001,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
//===============================================================================
//  Pin Declarations
//===============================================================================
//Inputs:
int nesData       = 4;    // The data pin for the NES controller

//Outputs:
int nesClock      = 2;    // The clock pin for the NES controller
int nesLatch      = 3;    // The latch pin for the NES controller

//===============================================================================
//  Initialization
//===============================================================================
int lower[16];
void setup() 
{
  
  // Set appropriate pins to inputs
  pinMode(nesData, INPUT);
  
  // Set appropriate pins to outputs
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);
  
  // Set initial states
  digitalWrite(nesClock, LOW);
  digitalWrite(nesLatch, LOW);
  // Setup for LCD
  lcd.begin(16,2);
  lcd.createChar(6, character);
  lcd.createChar(5, charjump1_head);
  lcd.createChar(4, charjump1_body);
  lcd.createChar(3, charjump2_upper);
  lcd.createChar(2, charjump2_lower);
  lower[0] = 1;
  for (int i = 1; i <= 15; i++) {
    lower[i] = 0;
  }
}

//===============================================================================
//  Main
//===============================================================================
int loc = 15;
int score = 0;
void loop() 
{
  // This function call will return the states of all NES controller's register
  // in a nice 8 bit variable format. Remember to refer to the table and
  // constants above for which button maps where!
  nesRegister = readNesController();
  
  String stringOne = "Final score: ";
  String result = stringOne + score;
  
  
  // Slight delay before we debug what was pressed so we don't spam the
  // serial monitor.
  lcd.setCursor(0, 1);
  lcd.print((char)6);
  lcd.setCursor(loc+1, 1);
  lcd.print(" ");
  lcd.setCursor(loc, 1);
  lcd.print("/");
  if (bitRead(nesRegister, A_BUTTON) == 0) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print((char)6);
    delay(50);
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(1, 1);
    lcd.print(" ");
  }
  if (loc == 0 || loc == 1) {
    if (bitRead(nesRegister, A_BUTTON) == 0) {
      score++;
      String stringOne = "Final score: ";
      String result = stringOne + score;
      loc = 15;
    }
    else {
      lcd.setCursor(0, 1);
      lcd.print(result);
      delay(1000);
      lcd.clear();
      loc = 15;
      score = 0;
    }
  }
  loc--;
  delay(150);
  // To give you an idea on how to use this data to control things for your
  // next project, look through the serial terminal code below. Basically,
  // just choose a bit to look at and decide what to do whether HIGH (not pushed)
  // or LOW (pushed). What is nice about this test code is that we mapped all
  // of the bits to the actual button name so no useless memorizing!
  
    
  /*if (bitRead(nesRegister, B_BUTTON) == 0)
    
  if (bitRead(nesRegister, START_BUTTON) == 0)
  
  if (bitRead(nesRegister, SELECT_BUTTON) == 0)
    
  if (bitRead(nesRegister, UP_BUTTON) == 0)
    
  if (bitRead(nesRegister, DOWN_BUTTON) == 0)
    
  if (bitRead(nesRegister, LEFT_BUTTON) == 0)
  
  if (bitRead(nesRegister, RIGHT_BUTTON) == 0)*/
}

//===============================================================================
//  Functions
//===============================================================================
///////////////////////
// readNesController //
///////////////////////
byte readNesController() 
{  
  // Pre-load a variable with all 1's which assumes all buttons are not
  // pressed. But while we cycle through the bits, if we detect a LOW, which is
  // a 0, we clear that bit. In the end, we find all the buttons states at once.
  int tempData = 255;
    
  // Quickly pulse the nesLatch pin so that the register grab what it see on
  // its parallel data pins.
  digitalWrite(nesLatch, HIGH);
  digitalWrite(nesLatch, LOW);
 
  // Upon latching, the first bit is available to look at, which is the state
  // of the A button. We see if it is low, and if it is, we clear out variable's
  // first bit to indicate this is so.
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, A_BUTTON);
    
  // Clock the next bit which is the B button and determine its state just like
  // we did above.
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, B_BUTTON);
  
  // Select button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, SELECT_BUTTON);

  // Start button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, START_BUTTON);

  // Up button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, UP_BUTTON);
    
  // Down button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, DOWN_BUTTON);

  // Left button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, LEFT_BUTTON);  
    
  // Right button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, RIGHT_BUTTON);
    
  // After all of this, we now have our variable all bundled up
  // with all of the NES button states.*/
  return tempData;
}

bool buttonPress(byte data) {
  if (bitRead(data, A_BUTTON) == 0) {
    return true;
  }
  else {
    return false;
  }
}

void spawn() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print((char)6);
}

void spawnEnemy() {
  for(int i = 15; i > 12; i--) {
    lcd.setCursor(i, 1);
    lcd.print("/");
    lcd.setCursor(i+1, 1);
    lcd.print(" ");
    delay(150);
  }
}

void shoot() {
  lcd.setCursor(1, 1);
  lcd.print(">");
  for(int i = 2; i <= 12; i++) {
    lcd.setCursor(i-1, 1);
    lcd.print(" ");
    lcd.setCursor(i, 1);
    lcd.print(">");
    delay(150);
  }
  lcd.setCursor(12, 1);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print(" ");
}

/*void jumpAnimation() {
  lcd.setCursor(0,1); // frame 1
  lcd.print((char)4);
  lcd.setCursor(0,0);
  lcd.print((char)5);
  delay(150);
  lcd.clear();
  lcd.setCursor(0,1); // frame 2
  lcd.print((char)2);
  lcd.setCursor(0,0);
  lcd.print((char)3);
  delay(150);
  lcd.clear();
  lcd.setCursor(0,0); // frame 3 - airborne
  lcd.print((char)6);
  delay(150);
  lcd.clear();
  lcd.setCursor(0,1); // frame 4 - begin descent
  lcd.print((char)2);
  lcd.setCursor(0,0);
  lcd.print((char)3);
  delay(150);
  lcd.clear();
  lcd.setCursor(0,1); // frame 5
  lcd.print((char)4);
  lcd.setCursor(0,0);
  lcd.print((char)5);
  delay(150);
  lcd.clear();
  lcd.setCursor(0,1); // end animation
  lcd.print((char)6);
}

void launchObject() {
  for(int i = 15; i >= 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(i, 1);
    lcd.print("/");
    lcd.setCursor(i+1, 1);
    lcd.print(" ");
    
    delay(150);
  }
}

bool checkCollision() {
  
}*/
