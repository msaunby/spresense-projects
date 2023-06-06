#include <MP.h>
#include <Wire.h>
#include "mpu_funcs.h"
#include "gps_funcs.h"
//#include "mlx_funcs.h"


void setup() {
// Use Spresense cores
  int ret = 0;
  int subid;

  for(subid = 1; subid <= 4; subid++){
    ret = MP.begin(subid);
    if(ret < 0){
      MPLog("\nMPLog %d %d\n", subid, ret);
    }
  }
  MP.RecvTimeout(MP_RECV_POLLING);

  Serial.begin(115200); //(115200);
  gps_setup();

  Wire.begin();

  mpu_setup();

  // Maximum speed on default I2C interface is 400kHz
  Wire.setClock(TWI_FREQ_400KHZ);
}

void printAccel(xyzFloat &g)
{
  printf("{\"accel\":\"%.2f,%.2f,%.2f\"}\n",
         g.x, g.y, g.z);
}

void printGyr(xyzFloat &gy)
{
  printf("{\"gyr\":\"%.1f,%.1f,%.1f\"}\n",
         gy.x, gy.y, gy.z);
}

void printMag(xyzFloat &mag)
{
  printf("{\"mag\":\"%.1f,%.1f,%.1f\"}\n",
         mag.x, mag.y, mag.z);
}


bool getImageData = false;

void loop() {
  int ret, subid;
  int8_t msgid;
  struct MyPacket{
    int status;
    float *data;
  }
  *packet;

  // Receive sub-core messages
  for(subid=1; subid <= 1; subid++) {
    ret = MP.Recv(&msgid, &packet, subid);
    if (ret > 0){
      Serial.print(msgid);
      Serial.println("*********************************************************** image *****");
      packet->status = 0;
    }
  }

  xyzFloat gValue = myMPU9250.getGValues();
  xyzFloat gyrValue = myMPU9250.getGyrValues();
  xyzFloat magValue = myMPU9250.getMagValues();
  float temp = myMPU9250.getTemperature();
  float resultantG = myMPU9250.getResultantG(gValue);

  printAccel(gValue);
  //printGyr(gyrValue);
  //printMag(magValue);
  //printMPU();

  printGps();

  delay(10);
}
