#define MAINMENUINIT 1
#define MAINMENU 2
#define RUNMENUINIT 3
#define RUNMENU 4
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_LiquidCrystal.h>
//typedef Adafruit_LiquidCrystal LiquidCrystal;
//LiquidCrystal lcd(0); // Initialises LCD on default address #0. Alternatively, change to 3 and jump A0 & A1.

//#include <LiquidCrystal.h>
// select the pins used on the LCD panel
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

LiquidCrystal_I2C lcd(0x27,16,2); 

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

unsigned int state = MAINMENU;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(26);      // read the value from the sensor, this is using pin 26 on the Teensy
 if (adc_key_in > 1500) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 500)  return btnLEFT; 
 if (adc_key_in < 700)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this...
}

void setup() {
  // put your setup code here, to run once:
  // Initialise IO Pins
  delay(100);
  //lcd.begin(16,2);
  lcd.init();
  delay(100);
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,1);
  
  //lcd.setCursor(1,1); // Sets cursor to 1st row, 1st character
  //lcd.noCursor();
  lcd.print("Initialising");
  lcd.setCursor(0,2); // Sets cursor to 2nd row, 1st character
  lcd.print("Please wait...");
  delay(500);
  //lcd.setCursor(1,1);
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
      lcd.clear();
      //lcd.setCursor(1,1);
      lcd.print("Ready to Run!"); // "Passed Time".
      lcd.setCursor(0,2);
      //lcd.print("Press SELECT");
      if(read_LCD_buttons() == btnSELECT){
        state = RUNMENUINIT;
      }
      delay(100);
      break;
    case RUNMENUINIT:
      lcd.clear();
      lcd.print("Intialising...");
      //unsigned int startTime = millis();
      delay(2500);
      state = RUNMENU;
      break;
    case RUNMENU:
      //NOTE: No such thing as try catch on arduino. Requires manual error handling (i.e. checking whether actions are possible)
      lcd.clear();
      lcd.print("Running for");
      lcd.setCursor(0,2);
      //lcd.print((millis()-startTime)/1000);
      lcd.print(millis()/1000-3);
      lcd.setCursor(4,2);
      lcd.print("seconds");
      delay(500);
      break;
  }
}
