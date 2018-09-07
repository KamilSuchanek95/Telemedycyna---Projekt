
/* include the SD library: */
#include <stdio.h>
#include <SPI.h>
#include <SD.h>

#define MAX_NUMBER_OF_FILE 288
#define MAX_NUMBER_OF_SAMPLE 60000
char fileName[12];
int kontrolka = 0;
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

}

void loop(void) {
  Serial.println("Loop Start");
  
  numberFile +=1;
  
  sprintf(fileName,"EKG%d.txt",numberFile);
  
  Serial.print(fileName);Serial.println(" ");
  
  if(numberFile<=MAX_NUMBER_OF_FILE){
    
    if(!SD.exists(fileName)){ /* Jeśli taki plik jeszcze nie istnieje na karcie */
      
      file = SD.open(fileName,FILE_WRITE); /* Stwórz ten plik */
      
      if(file){ /* Jeśli utworzenie pliku się powiodło */
        
        Serial.println("Utworzono plik: ");Serial.println(fileName);
        
        while(kontrolka!=2){ /* Zapisz tylko 5 minut zapisu! */
          if(numberSample>=30000){numberSample=0;kontrolka+=1;}
          numberSample +=1;Serial.println(numberSample);
          file.print(String(analogRead(A0)));file.print(",");delay(5);  
              
        }
        kontrolka =0;
        file.close();  
          
        }else{Serial.println("Utworzenie pliku się nie powiodło!");}
    }else{Serial.println("Taki plik już istnieje!");}    
  }else{Serial.println("Zapis na dzień zakończony!");}



}
