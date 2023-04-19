#define MAINMENUINIT 1
#define MAINMENU 2
#define RUNMENUINIT 3
#define RUNMENU 4
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Adafruit_LiquidCrystal.h>
typedef Adafruit_LiquidCrystal LiquidCrystal;
LiquidCrystal lcd(0); // Initialises LCD on default address #0. Alternatively, change to 3 and jump A0 & A1.
unsigned int state = MAINMENU;


void setup() {
  // put your setup code here, to run once:
  // Initialise IO Pins


  // Initialise LCD Screen
  if(!lcd.begin(16,2)){ // Initialise LCD, returning if successful or not.
    // If LCD is not working or not connected, and abort.
    while(1);
  }
  lcd.clear();
  lcd.setCursor(1,1); // Sets cursor to 1st row, 1st character
  lcd.noCursor();
  lcd.println("Initialising");
  lcd.setCursor(1,2); // Sets cursor to 2nd row, 1st character
  lcd.print("Please wait...");
  lcd.setCursor(1,1);
  // Initialise Sensors (WHEN APPLICABLE)
  
  // Initialise Timer (WHEN APPLICABLE)
}

void loop() {
  // put your main code here, to run repeatedly:
  // Activate certain code depending on the current state.
  switch(state){
    case MAINMENUINIT:
      lcd.clear();
      lcd.println("Running..."); // "Passed Time".
      lcd.setCursor(1,2); // Sets cursor to column 1 row 2, ready for first pass time reading on line 2.
      state = MAINMENU;
      break;
    case MAINMENU:

      break;
    case RUNMENUINIT:

      state = RUNMENU;
      break;
    case RUNMENU:
      //NOTE: No such thing as try catch on arduino. Requires manual error handling (i.e. checking whether actions are possible)

      break;
  }
}