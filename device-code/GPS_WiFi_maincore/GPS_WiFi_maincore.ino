#include <MP.h>

//#include <WiFiEspAT.h>

#include "gps_funcs.h"

#define USE_SD 1

#if USE_SD
#include <SDHCI.h>
#include <File.h>
SDClass SD;  /**< SDClass object */ 
File myFile; /**< File object */ 
#endif

void setup() {
// Use Spresense cores
  int ret = 0;
  int subid;

  Serial.begin(115200);

#if USE_SD
    while (!SD.begin()) {
    ; /* wait until SD card is mounted. */
  }

  /* Create a new directory */
  SD.mkdir("gps/");
  /* Open the file. Note that only one file can be open at a time,
     so you have to close this one before opening another. */
  myFile = SD.open("gps/location.json", FILE_WRITE);
#endif

  for(subid = 1; subid <= 4; subid++){
    ret = MP.begin(subid);
    if(ret < 0){
      MPLog("\nMPLog %d %d\n", subid, ret);
    }
  }
  MP.RecvTimeout(MP_RECV_POLLING);


  gps_setup();

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
  #if USE_SD
  // Write to SD card and grab the next reading too.
  myFile.println(text);
  text = writeGps();
  myFile.println(text);
  myFile.flush();
  #endif
  sleep(0.9);
}
