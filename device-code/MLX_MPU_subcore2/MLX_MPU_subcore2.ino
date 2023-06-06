#include <MP.h>
#include <Wire.h>
#include "mpu_funcs.h"

struct MyPacket{
    int status;
    xyzFloat *data;
  } packet;

void setup() {
  packet.status = 0;
  MP.begin();

  //Wire.begin();
  //Wire.setClock(TWI_FREQ_400KHZ);

  //Wire1.begin();
  //Wire1.setClock(TWI_FREQ_400KHZ);
  delay(1000);
  mpu_setup();
}

void send_message(const int id, const xyzFloat *msg){
  int ret;
  if (packet.status == 0){
    packet.status = 1;
    packet.data = msg;
    ret = MP.Send(id, &packet);
  }
}
void printAccel(xyzFloat &g)
{
  MPLog("{\"accel\":\"%.2f,%.2f,%.2f\"}\n",
         g.x, g.y, g.z);
}

void loop() {
  //xyzFloat gValue = myMPU9250.getGValues();
  //send_message(21, &gValue);
  delay(100);
}
