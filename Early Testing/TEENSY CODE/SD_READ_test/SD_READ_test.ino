#include <SPI.h>
#include <SD.h>

const int chipSelect = BUILTIN_SDCARD;


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

}

void loop() {
  
  File dataFile = SD.open("datalog.txt", FILE_READ);

    if (!dataFile) {
      Serial.println("Error opening datalog.txt for reading");
      while (1);
    }

    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }

    dataFile.close();
    while (1);
}
