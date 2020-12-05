#include <SD.h>

#define MAX_NUMBER_OF_FILE 300 //1440 /* maksymalna liczba rekordów */
/* 24h * 60min = 1440min */
#define MAX_NUMBER_OF_SAMPLE 12000 /* maksymalna liczba próbek w rekordzie */
/* 200Hz * 60s = 12000 próbek */
char fileName[12];
const int chipSelect = D8; /* D8 dla NodeMCU - oznaczenia pinów z literą D */
int numberFile = 0; /* inicjalizacja zmiennej */
File file; /* inicjalizacja zmiennej */
int numberSample = 0; /* inicjalizacja zmiennej */

void setup() {
  delay(5000); /* odczekaj 5s przed rozpoczęciem pracy */

  Serial.begin(9600); /* rozpocznij pracę z portem z prędkością 9600baud */
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

if(numberFile<MAX_NUMBER_OF_FILE){
  
  numberFile +=1; /* nowy numer rekordu */
  sprintf(fileName,"EKG%d.txt",numberFile); /* utworzenie nazwy rekordu */
  
  Serial.print(fileName);Serial.println(" ");/* informacja o pracy z danym plikiem na port */
  
    if(!SD.exists(fileName)){ /* Jeśli taki plik jeszcze nie istnieje na karcie */
      
      file = SD.open(fileName,FILE_WRITE); /* Stwórz ten plik */
      
      if(file){ /* Jeśli utworzenie pliku się powiodło */
        
        Serial.println("Utworzono plik: ");Serial.println(fileName);

        while (numberSample < MAX_NUMBER_OF_SAMPLE){
          numberSample +=1;Serial.println(analogRead(A0));
          file.print(String(analogRead(A0)));file.print(",");delay(5);  
          /* delay([ms]) da częstotliwość 200Hz */
        }
        file.close();  
          
        }else{Serial.println("Utworzenie pliku się nie powiodło!");}
    }else{Serial.println("Taki plik już istnieje!");}    
  }else{Serial.println("Zapis na dzień zakończony!");ESP.deepSleep(0);}
}
