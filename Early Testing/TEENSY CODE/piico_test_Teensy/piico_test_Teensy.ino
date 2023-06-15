#include <Wire.h>
#include "SparkFun_MS5637_Arduino_Library.h"

MS5637 barometricSensor;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Pressure Sensor MS5637 Testing");

  Wire1.begin(); // this is used for SDA1 & SCL1, pins 17 & 16.

  if (barometricSensor.begin(Wire1) == false) // Wire1 being passed in
  {
    Serial.println("MS5637 sensor did not respond. Please check wiring.");
    while(1);
  }
}

void loop(void) {
float temperature = barometricSensor.getTemperature();
  float pressure = barometricSensor.getPressure();

  Serial.print("Temperature=");
  Serial.print(temperature, 1);
  Serial.print("(C)");

  Serial.print(" Pressure=");
  Serial.print(pressure, 3);
  Serial.print("(hPa or mbar)");

  Serial.println();

  delay(100);
}
