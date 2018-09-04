
#include <SPI.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {
  pinMode(CS, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }  
  digitalWrite(CS, HIGH);/??

  SPI.begin();
}

void loop() {
  
}
