#include <Wire.h>
#include <SPI.h>

#include <DFRobot_BMP3XX.h>

DFRobot_BMP388_I2C sensor(&Wire, sensor.eSDOVDD);

#define CALIBRATE_ABSOLUTE_DIFFERENCE


void setup(void) {
    Serial.begin(9600);
    int rslt;
    while( ERR_OK != (rslt = sensor.begin()) ){
      if(ERR_DATA_BUS == rslt){
        Serial.println("Data bus error!!!");
      } else if(ERR_IC_VERSION == rslt){
        Serial.println("Chip versions do not match!!!");
      }
      delay(3000);
    }
    Serial.println("Begin ok!");

    while( !sensor.setSamplingMode(sensor.eUltraPrecision) ){
    Serial.println("Set samping mode fail, retrying....");
    delay(3000);
    }
    delay(100);
    #ifdef CALIBRATE_ABSOLUTE_DIFFERENCE
    if( sensor.calibratedAbsoluteDifference(10.0) ){
    Serial.println("Absolute difference base value set successfully!");
  }
  #endif
  /* Get the sampling period of the current measurement mode, unit: us */
  float sampingPeriodus = sensor.getSamplingPeriodUS();
  Serial.print("samping period : ");
  Serial.print(sampingPeriodus);
  Serial.println(" us");

  /* Get the sampling frequency of the current measurement mode, unit: Hz */
  float sampingFrequencyHz = 1000000 / sampingPeriodus;
  Serial.print("samping frequency : ");
  Serial.print(sampingFrequencyHz);
  Serial.println(" Hz");

  Serial.println();
  delay(1000);
}

void loop() {
  /* Read currently measured temperature date directly, unit: °C */
  float temperature = sensor.readTempC();
  Serial.print("temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  /* Directly read the currently measured pressure data, unit: pa */
  float Pressure = sensor.readPressPa();
  Serial.print("Pressure : ");
  Serial.print(Pressure);
  Serial.println(" Pa");
  
  Serial.println();
  delay(1000);
}
