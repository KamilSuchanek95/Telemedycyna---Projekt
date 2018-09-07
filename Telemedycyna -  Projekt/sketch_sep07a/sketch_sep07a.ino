/* include the SD library: */
#include <stdio.h>
#include <SPI.h>
#include <SD.h>

#define MAX_NUMBER_OF_FILE 288
#define MAX_NUMBER_OF_SAMPLE 60000
char fileName[12];

const int chipSelect = 10;
int numberFile = 0;
File file;
int numberSample = 0;

void setup() {
  delay(5000);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if(!(SD.begin(chipSelect))){
    Serial.println("Nie udało się zainicjować działania modułu czytnika kart SD.");
    while(1);
  }
  else{
    Serial.println("Moduł czytnika kart SD zainicjowany.");
  }

file = SD.open("EKG4.txt",FILE_READ);

}
int mia = 0;
void loop() {
  if(file){}else{Serial.print("!");}
  
  mia +=100*(int)file.read();
  mia +=10*(int)file.read();
  mia +=(int)file.read();
  file.read();
Serial.println(mia);
mia = 0;




}
