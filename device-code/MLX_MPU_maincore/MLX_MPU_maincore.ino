#include <MP.h>
//#include <Wire.h>
#include <SDHCI.h>
#include <File.h>
#include "gps_funcs.h"

SDClass SD;  /**< SDClass object */ 

File myFile; /**< File object */ 

void setup() {
// Use Spresense cores
  int ret = 0;
  int subid;

  Serial.begin(115200);

    while (!SD.begin()) {
    ; /* wait until SD card is mounted. */
  }

  /* Create a new directory */
  SD.mkdir("gps/");

  for(subid = 1; subid <= 4; subid++){
    ret = MP.begin(subid);
    if(ret < 0){
      MPLog("\nMPLog %d %d\n", subid, ret);
    }
  }
  MP.RecvTimeout(MP_RECV_POLLING);


  gps_setup();
  /* Open the file. Note that only one file can be open at a time,
     so you have to close this one before opening another. */
  myFile = SD.open("gps/location.json", FILE_WRITE);
}

bool getImageData = false;

struct MyPacket{
    int status;
} *packet;

void loop() {
  int ret, subid;
  int8_t msgid;
  char *text;

  // Receive sub-core messages
  for(subid=1; subid <= 3; subid++) {
    ret = MP.Recv(&msgid, &packet, subid);
    if (ret > 0){
      printGps();
    } 
  }
  text = writeGps();
  Serial.println(text);
  myFile.println(text);
  myFile.flush();
  sleep(10);
}
