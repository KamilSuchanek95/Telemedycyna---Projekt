
/* include the SD library: */
#include <SPI.h>
#include <SD.h>

/* set up variables using the SD utility library functions: */
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;

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

  /*File dataFile = SD.open("Test1.txt",FILE_WRITE);
  
  if(dataFile){
    dataFile.print(1);dataFile.print(",");Serial.println(dataFile.available());
    dataFile.print(2);dataFile.print(",");Serial.println(dataFile.available());
    dataFile.print(3);dataFile.print(",");Serial.println(dataFile.available());
    dataFile.print(4);dataFile.print(",");Serial.println(dataFile.available());
    dataFile.print(5);dataFile.print(",");Serial.println(dataFile.available());
    dataFile.close();
  }else{Serial.println("Wystąpił błąd poczas kreacji pliku.");}
  */
  if(SD.exists("Test1.txt")){
    Serial.println("Plik Test1.txt istnieje.");
  }else{Serial.println("Plik Test1.txt NIE istnieje.");}

  File dataFile = SD.open("Test1.txt",FILE_READ);
  if(dataFile){
    while(dataFile.available()){
      Serial.write(dataFile.read());
      //Serial.print(dataFile.read());Serial.print(dataFile.read());Serial.println(dataFile.available());
      //Serial.print(dataFile.read());Serial.print(dataFile.read());Serial.println(dataFile.available());
      //Serial.print(dataFile.read());Serial.print(dataFile.read());Serial.println(dataFile.available());
      //Serial.print(dataFile.read());Serial.print(dataFile.read());Serial.println(dataFile.available());
      
    }dataFile.close();
  }else{Serial.println("Wystąpił błąd poczas odczytu pliku.");}

  
#if 0
  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }

  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
#endif
}

void loop(void) {
}
