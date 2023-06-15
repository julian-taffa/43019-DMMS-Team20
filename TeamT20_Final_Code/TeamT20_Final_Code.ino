#define MAINMENUINIT 1
#define MAINMENU 2
#define RUNMENU 3
#define SAVETOSD 4

#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Wire.h>                             // for all of the i2c devices
#include <LiquidCrystal_I2C.h>                // for the i2c LCD backpack
#include "SparkFun_MS5637_Arduino_Library.h"  // for piico sensors
#include "DFRobot_I2C_Multiplexer.h"          // i2c multiplexer
#include <SPI.h>                              // for use with the SD card
#include <SD.h>                               // direct library for SD card

const int chipSelect = BUILTIN_SDCARD;  // we are using the Teensy 4.1's onboard SD card

const int numOfSensors = 8;  // the number of sensors per strip

const int numOfStrips = 1;  // the number of strips

const int bufferSize = 10000;  // the number of readings that can safely be stored in RAM before they have to be written to the SD card.
                               // The maths was assuming we can use half the RAM (half of 1024 KB is 512 KB), and each SensorData reading is 4*8 + 4 = 36 bytes.
                               // 512,000 / 36 =  14,200 readings. So we went with 10,000 to be nice and safe

int MAX_DATA_SIZE = 108000;  // this assumes 10Hz readings for 3 hours (10Hz * 60 sec/min * 180 min)

int ButtonRED = PIND2;
int Button2 = PIND3;
int Button3 = PIND4;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // initialising the LCD via the i2c backpack

MS5637 barometricSensor;  // initialising piico sensor

DFRobot_I2C_Multiplexer i2cMultiplexer(&Wire1, 0x70);  // initialising multiplexer

unsigned long lastLoopTime = 0;  // this is used to check whether the RUNMENU does actually work at a rate of 10Hz

unsigned int state = MAINMENU;

unsigned int debounce = 0;
unsigned long debounceTime = 0;
unsigned long debounceDelay = 50;

struct SensorData {  // this is a struct to hold the sensor data and timestamps
  float readings[numOfSensors];
  unsigned long timestamp;
};

char filename[20];  // the contents of 'filename' is what the .txt file will be called

SensorData sensorDataBuffer[bufferSize];  // create a SensorData struct the same size as the buffer

int bufferCount = 0;     // counter for which item the buffer is up to
int saveCount = 0;       // counter for how many times the program has saved to the SD card, when this hits 11 the program will have reached max
bool finalSave = false;  // flag for whether the current save to SD is the final one

void setup() {

  Serial.begin(9600);
  //Wire.begin()  // we don't need to begin Wire SDA and SCL (pins 18 & 19) because the lcd.init() does this automatically
  Wire1.begin();  // this is used for SDA1 & SCL1, pins 16 & 17, for the multiplexer

  pinMode(ButtonRED, INPUT_PULLUP);  // this is ButtonRED on Pin D2, current the only used button
  pinMode(Button2, INPUT_PULLUP);    // this is Button2 on Pin D3 // CURRENTLY UNUSED
  pinMode(Button3, INPUT_PULLUP);    // this is Button3 on Pin D4 // CURRENTLY UNUSED

  delay(100);
  lcd.init();
  delay(100);       // some testing found that adding these small delays helped with LCD initialisation
  lcd.clear();      // there shouldn't be anything on the LCD but clear it just in case
  lcd.backlight();  // ensure the LCD has its backlight on
  lcd.setCursor(0, 0);
  lcd.print("Initialising SD");
  lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
  lcd.print("Please wait...");

  // see if the SD card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SD Card failed.");
    lcd.setCursor(0, 1);
    lcd.print("Check & Restart");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SD card");
  lcd.setCursor(0, 1);
  lcd.print("initialized!");
  delay(1000);  // small delay to show user that SD card was initialised // OPTIONAL


  int fileNumber = 0;
  // the following code goes through to check which files exist on the SD card, so that
  // the code is guaranteed to create a new, unique file and not overwrite an old one
  do {
    sprintf(filename, "datafile%d.txt", fileNumber);
    fileNumber++;
  } while (SD.exists(filename));

  // The following code shows the end user the filename created
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("\'");
  lcd.print(filename);
  lcd.print("\'.txt");
  lcd.setCursor(0, 1);
  lcd.print("File created");
  delay(2000);  // small delay to show user that file was created successfully // OPTIONAL

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initialising");
  lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
  lcd.print("Multiplexer(s)");

  // Initialize the multiplexer
  i2cMultiplexer.begin();

  unsigned int errorCount = 0;
  for (int i = 0; i < numOfSensors; i++) {  // go through all of the connected sensors on the strip

    i2cMultiplexer.selectPort(i);  // select the current sensor's port

    if (!barometricSensor.begin(Wire1)) {  // if the sensor cannot initialise successfully
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor");
      lcd.setCursor(0, 1);
      lcd.print("No.");
      lcd.print(i + 1);  // this tells the user which sensor has failed (adding 1 because of zero index)
      lcd.print(" Fail");
      errorCount++;  // add to the count of the errors
      delay(2000);
    }
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(numOfSensors - errorCount);
  lcd.print(" of ");
  lcd.print(numOfSensors);
  lcd.print(" Sensors");
  lcd.setCursor(0, 1);
  lcd.print("Operational");
  delay(2000);  // small delay to show user number of sensors activated successfully // OPTIONAL
  lcd.clear();
}


void loop() {
  switch (state) {  // Activate certain code depending on the current state.
    case MAINMENU:  // the default Menu mode, which just waits idle until the user presses the RED button

      lcd.setCursor(0, 0);
      lcd.print("Ready to Run!");
      lcd.setCursor(0, 1);
      lcd.print("Hold RED");

      if (!digitalRead(ButtonRED)) {
        if (debounce % 10 == 0) {  // we are using multiples of 10 for debouncing
          debounce++;
          lastLoopTime = millis();
          // the following LCD code is given here rather than in the RUNMENU
          // so that there is less time required for processing
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Running! Hold");
          lcd.setCursor(0, 1);
          lcd.print("RED to stop");
          state = RUNMENU;  // change the state to RUNMENU
        } else {
          debounce++;
        }
      }
      delay(100);
      break;

    case RUNMENU:
      if (!digitalRead(ButtonRED)) {
        if (debounce % 10 == 0) {  // we are using multiples of 10 for debouncing
          debounce++;
          lcd.clear();
          finalSave = true;  // update the system so that it knows this is the last save
          state = SAVETOSD;
        } else {
          debounce++;
        }
      }
      if (millis() - lastLoopTime >= 100) {  // only trigger this at a rate of 10Hz
        lastLoopTime = millis();             // update lastLoopTime

        for (int i = 0; i < numOfSensors; i++) {                                       // go through each of the sensors
          i2cMultiplexer.selectPort(i);                                                // select the correct multiplexer port
          sensorDataBuffer[bufferCount].readings[i] = barometricSensor.getPressure();  // read the pressure
        }
        sensorDataBuffer[bufferCount].timestamp = millis();
        bufferCount++;

        if (bufferCount == bufferSize) {
          state = SAVETOSD;
        }
      }
      break;
    case SAVETOSD:
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Writing");
        lcd.setCursor(0, 1);
        lcd.print("To SD Card");

        File dataFile = SD.open(filename, FILE_WRITE);
        if (dataFile) {
          for (int i = 0; i < bufferCount; i++) {                             // go through each group of readings + timestamp
            for (int j = 0; j < numOfSensors; j++) {                          // go through each individual sensor reading per group
              dataFile.print(String(sensorDataBuffer[i].readings[j]) + ",");  // write the readings to the SD card, separated by commas
            }
            dataFile.print(String(sensorDataBuffer[i].timestamp));  // write the timestamp
            dataFile.println(String(i));                            // write the group number it is up to, and end the line
          }
          dataFile.close();
          saveCount++;  // increase the counter for save

          if (MAX_DATA_SIZE <= saveCount * bufferSize) {  // if we have the case where we have reached the 3-hour limit
            finalSave = true;
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          if (!finalSave) {  // if we do not yet want this to be the final save (ie, we only filled the buffer and RED wasn't pressed)
            // the following LCD code is given here rather than in the RUNMENU
            // so that there is less time required for processing
            lcd.print("Running! Press");
            lcd.setCursor(0, 1);
            lcd.print("RED to stop");
            bufferCount = 0;  // reset the bufferCount
            state = RUNMENU;  // change the state to RUNMENU
          } else {            // if finalSave is true, ie. we want this to be the last save to the SD card
            lcd.print("Save Complete!");
            lcd.setCursor(0, 1);
            lcd.print("Please Power Off");
            while (1)
              ;  // stay here until the device is powered off
          }

        } else {  // if the dataFile cannot be opened properly
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Writing Failed");
          lcd.setCursor(0, 1);
          lcd.print("Please Restart.");
        }
      }
      break;
  }
}
