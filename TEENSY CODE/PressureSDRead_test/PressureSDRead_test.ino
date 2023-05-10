#include <Wire.h>
#include "SparkFun_MS5637_Arduino_Library.h"  // for piico sensors
#include <DFRobot_BMP3XX.h>                   // for DFRobot sensors
#include <SPI.h>
#include <SD.h>

MS5637 barometricSensor;  // initialising piico sensor

DFRobot_BMP388_I2C sensor(&Wire2, sensor.eSDOVDD);  // initialising DFRobot sensor


const int chipSelect = BUILTIN_SDCARD;

unsigned long startTime;

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  Serial.println("card initialized.");

  Serial.println("Beginning sensor testing");

  Wire1.begin();  // this is used for SDA1 & SCL1, pins 17 & 16.
  Wire2.begin();  // this is used for SDA2 & SCL2, pins 24 & 25.

  // Initialise the Piico sensor
  if (barometricSensor.begin(Wire1) == false)  // Wire1 being passed in
  {
    Serial.println("MS5637 sensor did not respond. Please check wiring.");
    while (1)
      ;
  }

  // Initialize the DFRobot_BMP388_I2C sensor
  int rslt;
  while (ERR_OK != (rslt = sensor.begin())) {
    if (ERR_DATA_BUS == rslt) {
      Serial.println("Data bus error!!!");
    } else if (ERR_IC_VERSION == rslt) {
      Serial.println("Chip versions do not match!!!");
    }
    delay(3000);
  }
  Serial.println("Begin ok!");

  startTime = millis();
}

void loop() {
  if (millis() - startTime >= 20000) {  // Check if 20 seconds have passed
    Serial.println("Finished 20 seconds of logging.");
    Serial.println("Reading datalog.txt contents:");

    File dataFile2 = SD.open("datalog2.txt", FILE_READ);

    if (!dataFile2) {
      Serial.println("Error opening datalog2.txt for reading");
      while (1);
    }

    while (dataFile2.available()) {
      Serial.write(dataFile2.read());
    }

    dataFile2.close();
    while (1);
  }

  float temperature1 = barometricSensor.getTemperature();
  float pressure1 = barometricSensor.getPressure();
  float temperature2 = sensor.readTempC();
  float pressure2 = sensor.readPressPa();

  Serial.print("T1= ");
  Serial.print(temperature1, 1);
  Serial.print(" (C), ");

  Serial.print("P1=");
  Serial.print(pressure1, 3);
  Serial.print(" (hPa) ");

  Serial.print("T2= ");
  Serial.print(temperature2, 1);
  Serial.print(" (C), ");

  Serial.print("P2=");
  Serial.print(pressure2, 3);
  Serial.print(" Pa ");
  Serial.println();

  File dataFile = SD.open("datalog2.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print("t1: ");
    dataFile.print(temperature1);
    dataFile.print(", p1: ");
    dataFile.print(pressure1);
    dataFile.print("t2: ");
    dataFile.print(temperature2);
    dataFile.print(", p2: ");
    dataFile.println(pressure2);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }

  delay(100);
}
