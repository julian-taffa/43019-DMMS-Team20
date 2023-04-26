#define MAINMENUINIT 1
#define MAINMENU 2
#define RUNMENUINIT 3
#define RUNMENU 4
#define PROCESSING 5

#define RIGHT 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define SELECT 4
#define NONE 5 

#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
//#include <Adafruit_LiquidCrystal.h>
#include <LiquidCrystal.h>
//typedef Adafruit_LiquidCrystal LiquidCrystal;
//LiquidCrystal lcd(0); // Initialises LCD on default address #0. Alternatively, change to 3 and jump A0 & A1.
LiquidCrystal lcd(8,9,4,5,6,7);
unsigned int state = MAINMENUINIT;
unsigned int debounce = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned int currentTime = 0;
char printTime[16];

int readAdcButton(){
  int adc_key_in = analogRead(0);
  if (adc_key_in > 1500) return NONE;
  if (adc_key_in < 50)   return RIGHT;  
  if (adc_key_in < 195)  return UP; 
  if (adc_key_in < 380)  return DOWN; 
  if (adc_key_in < 500)  return LEFT; 
  if (adc_key_in < 700)  return SELECT;   
  return NONE;  // when all others fail, return this...
}
bool readButton(unsigned int button){
  if(readAdcButton() == button){
    return true;
  }
  else{
    return false;
  }
}

void setup() {
  // put your setup code here, to run once:
  // Initialise IO Pins

  // Initialise LCD Screen
  //if(!lcd.begin(16,2)){ // Initialise LCD, returning if successful or not.
  //  // If LCD is not working or not connected, and abort.
  //  while(1);
  //}
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0); // Sets cursor to 1st row, 1st character
  lcd.noCursor();
  lcd.print("Initialising");
  delay(10);
  lcd.setCursor(0,1); // Sets cursor to 2nd row, 1st character
  lcd.print("Please wait...");
  // Initialise Sensors (WHEN APPLICABLE)
  
  // Initialise Timer (WHEN APPLICABLE)

  // Initialise Buttons (WHEN APPLICABLE)

  // DEMO CODE
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Activate certain code depending on the current state.
  switch(state){
    case MAINMENUINIT:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Aero Logger V0.1");
      lcd.setCursor(0,1);
      lcd.print("Press START");
      state = MAINMENU;
      break;
    case MAINMENU:
      if(readButton(SELECT)){
        if(debounce != 10){
          debounce++;
        }
        else{
          debounce++;
          state = RUNMENUINIT;
        }
      }
      else{
        debounce = 0;
      }
      break;
    case RUNMENUINIT:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Running..."); // "Passed Time".
      startTime = millis();
      state = RUNMENU;
      break;
    case RUNMENU:
      //NOTE: No such thing as try catch on arduino. Requires manual error handling (i.e. checking whether actions are possible)
      if(readButton(SELECT)){
        if(debounce != 10){
          debounce++;
        }
        else{
          debounce++;
          endTime = millis();
          state = PROCESSING;
        }
      }
      else{
        debounce = 0;
      }
      // INSERT RUNNING CODE HERE
      currentTime = millis() - startTime;
      //sprintf(printTime,"%u",currentTime/1000);
      lcd.setCursor(0,1);
      lcd.print(currentTime/1000);
      lcd.print(" sec");
      break;
    case PROCESSING:
      endTime = millis();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Saving Data...");
      lcd.setCursor(0,1);
      lcd.print("DO NOT SHUT DOWN");
      // INSERT CODE FOR PROCESSING AND SAVING DATA
      //DEMO CODE
      delay(10000);
      state = MAINMENUINIT;
      break;
  }
}
