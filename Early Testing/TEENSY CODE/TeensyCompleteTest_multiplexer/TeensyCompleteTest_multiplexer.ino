#define MAINMENUINIT 1
#define MAINMENU 2
#define RUNMENUINIT 3
#define RUNMENU 4
#define SAVETOSD 5

#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Wire.h>                             // for all of the i2c devices
#include <LiquidCrystal_I2C.h>                // for the i2c LCD backpack
#include "SparkFun_MS5637_Arduino_Library.h"  // for piico sensors
#include "DFRobot_I2C_Multiplexer.h"          // i2c multiplexer
#include <SPI.h>                              // for use with the SD card
#include <SD.h>                               // direct library for SD card

const int chipSelect = BUILTIN_SDCARD;

const int numOfSensors = 8;  // the number of sensors per strip

const int numOfStrips = 1;  // the number of strips

int MAX_DATA_SIZE = 108000;  // this assumes 10Hz readings for 3 hours (10Hz * 60 sec/min * 180 min)

int Button1 = PIND2;
int Button2 = PIND3;
int Button3 = PIND4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

MS5637 barometricSensor;  // initialising piico sensor

//DFRobot_I2CMultiplexer* i2cMultiplexers;  // initialising multiplexers via a pointer (if we need multiple, for multiple strips)
DFRobot_I2C_Multiplexer i2cMultiplexer(&Wire1, 0x70);  // initialising multiplexer

unsigned long startTime;

unsigned int state = MAINMENU;

unsigned int debounce = 0;

struct SensorData {  // this is a struct to hold the sensor data and timestamps
  float readings[numOfSensors];
  unsigned long timestamp;
};


//SensorData** sensorData;  // create a SensorData variable with the correct number of sensors. This is a pointer to a pointer, which will allow for a 2D array

SensorData* sensorData = new SensorData[0];
int sensorDataSize = 0;

void setup() {
  Serial.begin(9600);
  //Wire.begin();
  Wire1.begin();
  // put your setup code here, to run once:
  pinMode(Button1, INPUT_PULLUP);  // this is Button1 on Pin D2
  pinMode(Button2, INPUT_PULLUP);  // this is Button2 on Pin D3
  pinMode(Button3, INPUT_PULLUP);  // this is Button3 on Pin D4

  delay(100);
  lcd.init();
  delay(100);
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initialising SD");
  lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
  lcd.print("Please wait...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SD Card failed.");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SD card");
  lcd.setCursor(0, 1);
  lcd.print("initialized!");
  delay(2000);  // large delay to show user that SD card was initialised

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initialising");
  lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
  lcd.print("Multiplexer(s)");

  //Wire1.begin();  // this is used for SDA1 & SCL1, pins 16 & 16, for the multiplexer
  i2cMultiplexer.begin();
  // Initialize the multiplexers
  //for (int i = 0; i < numOfSensors; i++) {
  for (int i = 0; i < numOfSensors; i++) {
    delay(100);
    i2cMultiplexer.selectPort(i);
    delay(100);
    if (!barometricSensor.begin(Wire1)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor");
      lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
      lcd.print("No.");
      lcd.print(i + 1);
      lcd.print(" Fail");
      //Serial.println("Failed to initialize the I2C Multiplexer!");
      delay(1000);
    }
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("All Sensors");
  lcd.setCursor(0, 1);  // Sets cursor to 2nd row, 1st character
  lcd.print("Working!!!");
  while (1)
    ;

  // for (int j = 0; j < numOfStrips; i++) {
  //   for (int i = 0; i < numOfSensors; i++) {
  //     i2cMultiplexers[j].selectPort(i);  // select a port
  //     if (barometricSensor.begin(Wire1) == false) {
  //       lcd.clear();
  //       lcd.setCursor(0, 0);
  //       lcd.print("MS5637 sensor ");
  //       lcd.print(i);
  //       lcd.setCursor(0, 1);
  //       lcd.print("Stp: ");
  //       lcd.print(j);
  //       lcd.print("failed.");
  //     }
  //   }
  // }


  //Wire1.begin();  // this is used for SDA1 & SCL1, pins 17 & 16.
  //Wire2.begin();  // this is used for SDA2 & SCL2, pins 24 & 25.

  // // Initialise the Piico sensor
  // if (barometricSensor.begin(Wire1) == false)  // Wire1 being passed in
  // {
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("MS5637 sensor");
  //   lcd.setCursor(0, 1);
  //   lcd.print("failed.");
  //   while (1) {
  //     // Sensor failed, so don't do anything more - stay stuck here
  //   }
  // }

  //   // Initialize the DFRobot_BMP388_I2C sensor
  //   int rslt;
  //   while (ERR_OK != (rslt = sensor.begin())) {
  //     if (ERR_DATA_BUS == rslt) {
  //       lcd.clear();
  //       lcd.setCursor(0, 0);
  //       lcd.print("Data bus error");
  //       lcd.setCursor(0, 1);
  //       lcd.print("System stop.");
  //     } else if (ERR_IC_VERSION == rslt) {
  //       lcd.clear();
  //       lcd.setCursor(0, 0);
  //       lcd.print("Chip versions do");
  //       lcd.setCursor(0, 1);
  //       lcd.print("not match");
  //     }
  //     delay(3000);
  //   }
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("All systems");
  //   lcd.setCursor(0, 1);
  //   lcd.print("good to go!");
  //   delay(2000);
  // }
}
void loop() {
  // Activate certain code depending on the current state.
  switch (state) {
    case MAINMENU:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ready to Run!");
      lcd.setCursor(0, 1);
      lcd.print("Hold BTN1");
      if (!digitalRead(Button1)) {
        if (debounce != 10) {
          debounce++;
        } else {
          debounce++;
          startTime = millis();
          state = RUNMENU;
        }
      }
      delay(100);
      break;
    case RUNMENU:
      // if (millis() - startTime >= 20000) {  // Check if 20 seconds have passed
      //   Serial.println("Finished 20 seconds of logging.");
      //   Serial.println("Reading datalog3.txt contents:");
      //   lcd.clear();
      //   lcd.setCursor(0, 0);
      //   lcd.print("Finished");
      //   lcd.setCursor(0, 1);
      //   lcd.print("Datalogging");


      //   File dataFile2 = SD.open("datalog3.txt", FILE_READ);

      //   if (!dataFile2) {
      //     Serial.println("Error opening datalog3.txt for reading");
      //     while (1)
      //       ;
      //   }

      //   while (dataFile2.available()) {
      //     Serial.write(dataFile2.read());
      //   }

      //   dataFile2.close();
      //   lcd.clear();
      //   while (1) {
      //     lcd.setCursor(0, 0);
      //     lcd.print("Test");
      //     lcd.setCursor(0, 1);
      //     lcd.print("Complete!");
      //     delay(200);
      //   };
      // }

      // if (millis() - startTime < 20000) {

      // sensorData = (SensorData*)realloc(sensorData, (sensorDataSize + 1) * sizeof(SensorData));  // resize the memory for sensorData

      // Resize the memory for each strip's sensorData when a new data point is collected
      // for (int strip = 0; strip < numOfStrips; strip++) {
      //   sensorData[strip] = (SensorData*)realloc(sensorData[strip], (sensorDataSize + 1) * sizeof(SensorData));
      //   //sensorDataSize++;
      // }

      // for (int i = 0; i < numOfSensors; i++) {                                    // go through each of the sensors
      //   i2cMultiplexer.selectPort(i);                                             // select the correct multiplexer port
      //   sensorData[sensorDataSize].readings[i] = barometricSensor.getPressure();  // read the pressure
      // }
      // sensorData[sensorDataSize].timestamp = millis();  // read the time

      //   for (int strip = 0; strip < numOfStrips; strip++) {                                       // go through each strip
      //     for (int sensor = 0; sensor < numOfSensors; sensor++) {                                 // go through each sensor on the strip
      //       i2cMultiplexers[strip].selectPort(sensor);                                            // select the correct multiplexer port
      //       sensorData[strip][sensorDataSize].readings[sensor] = barometricSensor.getPressure();  // read the pressure
      //     }
      //     sensorData[strip][sensorDataSize].timestamp = millis();  // record the time
      //   }
      //   sensorDataSize++;  // increment the size of sensorData

      //   if (sensorDataSize >= MAX_DATA_SIZE) {  // if we've reached the max decided data size
      //     state = SAVETOSD;                     // then change the state to be the mode where we save the data back to the SD card
      //   }
      // } else {
      //   state = SAVETOSD;
      // }
      // float temperature1 = barometricSensor.getTemperature();
      // float pressure1 = barometricSensor.getPressure();
      // float temperature2 = sensor.readTempC();
      // float pressure2 = sensor.readPressPa();

      // File dataFile = SD.open("datalog3.txt", FILE_WRITE);

      // Serial.print("T1= ");
      // Serial.print(temperature1, 1);
      // Serial.print(" (C), ");

      // Serial.print("P1=");
      // Serial.print(pressure1, 3);
      // Serial.print(" (hPa) ");

      // Serial.print("T2= ");
      // Serial.print(temperature2, 1);
      // Serial.print(" (C), ");

      // Serial.print("P2=");
      // Serial.print(pressure2, 3);
      // Serial.print(" Pa ");
      // Serial.println();

      // if (dataFile) {
      //   dataFile.print("time: ");
      //   dataFile.print(millis() - startTime);
      //   dataFile.print(", t1: ");
      //   dataFile.print(temperature1);
      //   dataFile.print(", p1: ");
      //   dataFile.print(pressure1);
      //   dataFile.print(", t2: ");
      //   dataFile.print(temperature2);
      //   dataFile.print(", p2: ");
      //   dataFile.println(pressure2);
      //   dataFile.close();
      // } else {
      //   Serial.println("Error opening datalog3.txt");
      // }
      // //NOTE: No such thing as try catch on arduino. Requires manual error handling (i.e. checking whether actions are possible)
      // lcd.clear();
      // lcd.setCursor(0, 0);
      // lcd.print("System");
      // lcd.setCursor(0, 1);
      // //lcd.print((millis()-startTime)/1000);
      // lcd.print("Running...");
      // delay(500);
      // break;

    case SAVETOSD:
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Writing");
        lcd.setCursor(0, 1);
        lcd.print("To SD Card");
      //   char filename[15];
      //   int fileNumber = 0;
      //   // the following code goes through to check which files exist on the SD card, so that
      //   // the code is guaranteed to create a new, unique file and not overwrite an old one
      //   do {
      //     sprintf(filename, "datafile%d.txt", fileNumber);
      //     fileNumber++;
      //   } while (SD.exists(filename));
      // }
      // File dataFile = SD.open(filename, FILE_WRITE);
      // if (dataFile) {
      //   for (int i = 0; i < sensorDataSize; i++) {                    // go through each group of readings + timestamp
      //     for (int j = 0; j < numOfSensors) {                         // go through each individual sensor reading per group
      //       dataFile.print(String(sensorData[i].readings[j]) + ",");  // write the readings to the SD card, separated by commas
      //     }
      //     dataFile.print(String(sensorData[i].timestamp));  // write the timestamp
      //     dataFile.println(String(i));                      // write the group number it is up to, and end the line
      //   }
      //   dataFile.close();
      // }
      //     File dataFile = SD.open(filename, FILE_WRITE);
      //     if (dataFile) {
      //       for (int strip = 0; strip < numOfStrips; strip++) {                        // go through each strip
      //         for (int count = 0; count < sensorDataSize; count++) {                   // go through each set of readings + timestamp
      //           for (int j = 0; j < numOfSensors; j++) {                               // go through each individual sensor reading per group
      //             dataFile.print(String(sensorData[strip][count].readings[j]) + ",");  // write the readings to the SD card, separated by commas
      //           }
      //           // write the string number, count of what reading it is up to, timestamp, and then end the line
      //           dataFile.println("s" + String(strip) + ",no" + String(count) + "," + String(sensorData[strip][count].timestamp));
      //         }
      //       }
      //       dataFile.close();
      //     } else {
      //       lcd.setCursor(0, 0);
      //       lcd.print("Writing");
      //       lcd.setCursor(0, 1);
      //       lcd.print("Failed :(");
      //     }
      //     for (int i = 0; i < numOfStrips; i++) {  // free the memory by deleting all of the sensor data when the program closes
      //       delete[] sensorData[i];
      //     }
      //     delete[] sensorData; // delete the entire pointer to a pointer
      // }
  }
}
}
