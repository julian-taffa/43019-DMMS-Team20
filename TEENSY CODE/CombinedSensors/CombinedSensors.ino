#include <Wire.h>
#include "SparkFun_MS5637_Arduino_Library.h"
#include <SPI.h>
#include <SD.h>

MS5637 barometricSensor;

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

  Wire1.begin(); // this is used for SDA1 & SCL1, pins 17 & 16.

  if (barometricSensor.begin(Wire1) == false) // Wire1 being passed in
  {
    Serial.println("MS5637 sensor did not respond. Please check wiring.");
    while(1);
  }

  startTime = millis();
}

void loop() {
  if (millis() - startTime >= 20000) { // Check if 20 seconds have passed
    Serial.println("Finished 20 seconds of logging.");
    while(1); // hit an infinite loop to stop it functioning    
}

  // put your main code here, to run repeatedly:
  float temperature = barometricSensor.getTemperature();
  float pressure = barometricSensor.getPressure();

  Serial.print("Temperature=");
  Serial.print(temperature, 1);
  Serial.print("(C)");

  Serial.print(" Pressure=");
  Serial.print(pressure, 3);
  Serial.print("(hPa or mbar)");

  Serial.println();

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print("tp: ");
    dataFile.print(temperature);
    dataFile.print(", pr: ");
    dataFile.println(pressure);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }

  delay(100);
}
