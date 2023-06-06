#include <MP.h>
#include <Wire.h>
#include "mlx_funcs.h"
#include "mpu_funcs.h"

struct MyPacket {
  int status; 
  xyzFloat g, gyr, mag;
  paramsMLX90640 params;
  uint16_t mlx90640Frame1[834];
  uint16_t mlx90640Frame2[834];
} packet;

uint16_t mlx90640Frame1[834];
uint16_t mlx90640Frame2[834];

void setup() {
  packet.status = 0;
  MP.begin();

  Wire.begin();
  Wire.setClock(TWI_FREQ_400KHZ);

  Wire1.begin();
  Wire1.setClock(TWI_FREQ_400KHZ);

  mpu_setup(); //Wire
  mlx_setup(); //Wire1
}

void send_message(const int id, const xyzFloat g, const xyzFloat gyr, const xyzFloat mag) {
  int ret;
  if (packet.status == 0) {
    packet.status = 1;
    packet.g = g;
    packet.gyr = gyr;
    packet.mag = mag;
    packet.params = mlx1.getParams();
    ret = MP.Send(id, &packet);
  }
}

xyzFloat gValue, gyrValue, magValue;

void loop() {

  gValue = myMPU9250.getGValues();
  gyrValue = myMPU9250.getGyrValues();
  magValue = myMPU9250.getMagValues();

  if (mlx1.getFrameData(packet.mlx90640Frame1) == 0) {
    send_message(1, gValue, gyrValue, magValue);
  } else {
    send_message(-1, gValue, gyrValue, magValue);
  }

  gValue = myMPU9250.getGValues();
  gyrValue = myMPU9250.getGyrValues();
  magValue = myMPU9250.getMagValues();

  if (mlx2.getFrameData(packet.mlx90640Frame2) == 0) {
    send_message(2, gValue, gyrValue, magValue);
  } else {
    send_message(-2, gValue, gyrValue, magValue);
  }

}
