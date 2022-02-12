/*
I'm using the SdFat Library, primarily because I've had many issues with the Arduino IDE trying to use the Non ESP8266 
version of the SD Library when installed. This gets around that because they have different names and the functionality
seems to be the same as far as I can tell.

NOTE: I am not the original writer of pretty much any of this code, I just re-wrote alot of seperate code examples to 
be more framiliar to a typical "Arduino C" style, added more comments and combined them into one useage example.
*/ 

#include <SdFat.h> // https://github.com/greiman/SdFat
#include <SPI.h>

// PIN DEFINITIONS /////////////////////////////////////////////

#define SD_SCK_PIN          14  // Wimos pin D5
#define SD_MISO_PIN         12  // Wimos pin D6
#define SD_MOSI_PIN         13  // Wimos pin D7
#define SD_CHIP_SELECT_PIN  15  // Wimos pin D8

// VARIABLES ///////////////////////////////////////////////////

SdFat sd;
File myFile;
File myDir;

uint32_t cardSize;
uint32_t eraseSize;

void setup() {
  
  Serial.begin(115200);
  
  // REMOVE THESE LINES IF YOU WANT THIS TO WORK WITHOUT SERIAL OUTPUT REQUIRED!!
  while (!Serial)
    yield();

  initSDCard();
  
  // start examples
  dumpSDInfo();
  createDirectory();
  writeToFile();
  readFromFile();
  readDirectory();
  removeFolder();
}

void loop() {
    
}

void readDirectory() {

  Serial.println("");
  Serial.println("Reading directory test");

  // attempt to access the folder 'testFolder' directory information
  if (myDir.open("/testFolder")) {

    // loop through it's contents and open them one object at a time
    while (myFile.openNext(&myDir, O_RDONLY)) {
      
      myFile.printFileSize(&Serial);
      Serial.write(' ');
      myFile.printModifyDateTime(&Serial);
      Serial.write(' ');
      myFile.printName(&Serial);
      
      // if this is a directory display a forward slash at the end of the name
      if (myFile.isDir())
        Serial.write('/');
      
      Serial.println("");
      
      // make sure we close our access to the object when ew're done
      myFile.close();
    }
  
  } else {
    
    Serial.println("open myDir failed");
  }  
}

void removeFolder() {
  
  Serial.println("");
  Serial.println("Removing directory test");
  
  // Remove a folder called 'testFolder'
  // rmdir will return true if the directory was successfully removed
  if (sd.rmdir("testFolder")) {
    
    Serial.println("Removal of 'testFolder' completed!");
  
  } else {

    Serial.println("Removal of 'testFolder' failed");
  }
}

void createDirectory() {

  Serial.println("");
  Serial.println("Creating directory test");

  // if the folder already exists, don't bother creating it
  if (!sd.exists("testFolder")) {

    // attempt to access the root directory information
    if (myDir.open("/")) {

      // Create a new folder called 'testFolder'
      // mkdir will return true if it completes successfully
      if (sd.mkdir("testFolder")) {

        Serial.println(F("Created 'testFolder' successfully"));
      
      } else {

        Serial.println("Create 'testFolder' failed");
      }
      
    } else {
      
      Serial.println("Open 'testFolder' failed");
    }  
    
  } else {

    Serial.println("Folder 'testFolder' already exists!");
  }
}

void writeToFile() {

  Serial.println("");
  Serial.println("Writing to file test");

  // Open the object for access
  myFile = sd.open("testFolder/test.txt", FILE_WRITE);

  // Check if the file opened properly, if not it will be null
  if (myFile) {
  
    Serial.print("Writing to 'testFolder/test.txt'...");
    myFile.println("testing 1, 2, 3.");

    // Close the file when we're done. 
    // if you don't close the file, your changes will not get applied. 
    // Also, you can only have one file open at a time, so closing when you're finished is essential
    myFile.close();
    Serial.println("Write Complete.");
  
  } else {
    
    Serial.println("error opening test.txt");
  }
}

void readFromFile() {
  
  Serial.println("");
  Serial.println("Reading from file test");

  // Open the object for access
  myFile = sd.open("testFolder/test.txt");
  
  // Check if the file opened properly, if not it will be null
  if (myFile) {
  
    Serial.println("'testFolder/test.txt' contains:");

    // Read each line of the file, one at a time
    while (myFile.available())
      Serial.write(myFile.read());
    
    // Close the file when we're done. 
    // if you don't close the file, your changes will not get applied. 
    // Also, you can only have one file open at a time, so closing when you're finished is essential
    myFile.close();

  } else {
  
    Serial.println("error opening 'testFolder/test.txt'");
  }
}

void initSDCard() {

  uint32_t t = millis();

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(SD_CHIP_SELECT_PIN, SD_SCK_MHZ(50))) {
    
    Serial.println("sd.begin failed");
    return;
  }

  t = millis() - t;
  cardSize = sd.card()->cardSize();
  
  if (cardSize == 0) {
  
    Serial.println("cardSize failed");
    return;
  }
  
  Serial.print(F("\ninit time: "));
  Serial.print(t);
  Serial.println(" ms");
}

void dumpSDInfo() {
    
  Serial.print(F("SdFat version: "));
  Serial.println(SD_FAT_VERSION); 
  Serial.print(F("\nCard type: "));
  
  switch (sd.card()->type()) {
    
    case SD_CARD_TYPE_SD1:
      Serial.println(F("SD1"));
      break;
  
    case SD_CARD_TYPE_SD2:
      Serial.println(F("SD2"));
      break;
  
    case SD_CARD_TYPE_SDHC:
      if (cardSize < 70000000)
        Serial.println(F("SDHC"));
      else
        Serial.println(F("SDXC"));
      
      break;
  
    default:
      Serial.println(F("Unknown"));
  }
  
  if (!dumpCIDInformation())
    return;

  if (!dumpCSDInformation())
    return;

  uint32_t ocr;
  
  if (!sd.card()->readOCR(&ocr)) {
  
    Serial.println("\nreadOCR failed");
    return;
  }
  
  Serial.print(F("OCR: "));
  Serial.print("0X");
  Serial.println(ocr, HEX);
  
  if (!dumpPartitionInformation())
    return;
  
  if (!sd.fsBegin()) {
    Serial.println("\nFile System initialization failed.\n");
    return;
  }

  dumpVolumeInformation();
}

uint8_t dumpCIDInformation() {
  
  cid_t cid;
  
  if (!sd.card()->readCID(&cid)) {
    
    Serial.println("readCID failed");
    return false;
  }
  
  Serial.print(F("\nManufacturer ID: "));
  Serial.print("0X");
  Serial.println(int(cid.mid), HEX);
  Serial.println(F("OEM ID: ") + cid.oid[0] + cid.oid[1]);
  Serial.print(F("Product: "));
  
  for (uint8_t i = 0; i < 5; i++)
    Serial.print(cid.pnm[i]);
  
  Serial.println("");
  
  Serial.print(F("Version: "));
  Serial.print(int(cid.prv_n));
  Serial.print('.');
  Serial.println(int(cid.prv_m));
  
  Serial.print(F("Serial number: "));
  Serial.print("0X");
  Serial.println(cid.psn, HEX);
  
  Serial.print(F("Manufacturing date: "));
  Serial.print(int(cid.mdt_month));
  Serial.print('/');
  Serial.println((2000 + cid.mdt_year_low + 10 * cid.mdt_year_high));
  Serial.println("");
  return true;
}

uint8_t dumpCSDInformation() {
  
  csd_t csd;
  uint8_t eraseSingleBlock;
  
  if (!sd.card()->readCSD(&csd)) {
    
    Serial.println("readCSD failed");
    return false;
  }
  
  if (csd.v1.csd_ver == 0) {
    
    eraseSingleBlock = csd.v1.erase_blk_en;
    eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
  
  } else if (csd.v2.csd_ver == 1) {
  
    eraseSingleBlock = csd.v2.erase_blk_en;
    eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
  
  } else {
  
    Serial.println(F("csd version error"));
    return false;
  }
  
  eraseSize ++;
  Serial.print(F("cardSize: "));
  Serial.print(0.000512 * cardSize);
  Serial.println(F(" MB (MB = 1,000,000 bytes)"));

  Serial.print(F("flashEraseSize: "));
  Serial.print(int(eraseSize));
  Serial.println(F(" blocks"));
  
  Serial.print(F("eraseSingleBlock: "));
  
  if (eraseSingleBlock)
    Serial.println(F("true"));
  else
    Serial.println(F("false"));
  
  return true;
}

uint8_t dumpPartitionInformation() {
  
  mbr_t mbr;
  
  if (!sd.card()->readBlock(0, (uint8_t*)&mbr)) {
  
    Serial.println("read MBR failed");
    return false;
  }
  
  for (uint8_t ip = 1; ip < 5; ip++) {
  
    part_t *pt = &mbr.part[ip - 1];
    
    if ((pt->boot & 0X7F) != 0 || pt->firstSector > cardSize) {
    
      Serial.println(F("\nNo MBR. Assuming Super Floppy format."));
      return true;
    }
  }
  
  Serial.println(F("\nSD Partition Table"));
  Serial.println(F("part,boot,type,start,length"));
  
  for (uint8_t ip = 1; ip < 5; ip++) {
    
    part_t *pt = &mbr.part[ip - 1];
    Serial.print("0X");
    Serial.print(int(ip), HEX);
    Serial.print(',');
    Serial.print("0X");
    Serial.print(int(pt->boot), HEX);
    Serial.print(',');
    Serial.print("0X");
    Serial.print(int(pt->type), HEX);
    Serial.print(',');
    Serial.print(pt->firstSector);
    Serial.print(',');
    Serial.println(pt->totalSectors);
  }
  
  return true;
}

void dumpVolumeInformation() {
  
  Serial.print(F("\nVolume is FAT"));
  Serial.println(int(sd.vol()->fatType()));
  
  Serial.print(F("blocksPerCluster: "));
  Serial.println(int(sd.vol()->blocksPerCluster()));
  
  Serial.print(F("clusterCount: "));
  Serial.println(sd.vol()->clusterCount());
  
  Serial.print(F("freeClusters: "));
  uint32_t volFree = sd.vol()->freeClusterCount();
  Serial.println(volFree);
  
  float fs = 0.000512*volFree*sd.vol()->blocksPerCluster();
  Serial.print(F("freeSpace: "));
  Serial.print(fs);
  Serial.println(F(" MB (MB = 1,000,000 bytes)"));
  
  Serial.print(F("fatStartBlock: "));
  Serial.println(sd.vol()->fatStartBlock());
  
  Serial.print(F("fatCount: "));
  Serial.println(int(sd.vol()->fatCount()));
  
  Serial.print(F("blocksPerFat: "));
  Serial.println(sd.vol()->blocksPerFat());
  
  Serial.print(F("myDirDirStart: "));
  Serial.println(sd.vol()->rootDirStart());
  
  Serial.print(F("dataStartBlock: "));
  Serial.println(sd.vol()->dataStartBlock());
  
  if (sd.vol()->dataStartBlock() % eraseSize) {
  
    Serial.println(F("Data area is not aligned on flash erase boundaries!"));
    Serial.println(F("Download and use formatter from www.sdcard.org!"));
  }
}
