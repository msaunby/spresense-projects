
#include <Wire.h>
#include "mpu_funcs.h"
#include "gps_funcs.h"
#include "mlx_funcs.h"


void setup() {
  Serial.begin(1000000); //(115200);
  gps_setup();
  
  Wire.begin();
  Wire1.begin();


  mpu_setup();
  mlx_setup();

  // Maximum speed on default I2C interface is 400kHz
  Wire.setClock(TWI_FREQ_400KHZ);
  Wire1.setClock(TWI_FREQ_400KHZ);
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


bool getImageData = true;

void loop() {
  xyzFloat gValue = myMPU9250.getGValues();
  xyzFloat gyrValue = myMPU9250.getGyrValues();
  xyzFloat magValue = myMPU9250.getMagValues();
  float temp = myMPU9250.getTemperature();
  float resultantG = myMPU9250.getResultantG(gValue);

  printAccel(gValue);
  printGyr(gyrValue);
  printMag(magValue);

  printGps();

  if (getImageData) { 
    if (mlx1.getFrame(frame1) != 0) {
      Serial.println("Image 1 Failed");
    }

    if (mlx2.getFrame(frame2) != 0) {
      Serial.println("Image 2 Failed");
    }
  }

  if (getImageData) { 
  float t;
  Serial.print("{\"image1\":[");

  t = frame1[0];
  Serial.print(t, 1);
  for (uint16_t n=1; n<(24*32); n++) {
    t = frame1[n];
    Serial.print(",");
    Serial.print(t, 1);    
  }
  Serial.println("]}");
  
  Serial.print("{\"image2\":[");
  
  t = frame2[0];
  Serial.print(t, 1);
  for (uint16_t n=1; n<(24*32); n++) {
    t = frame2[n];
    Serial.print(",");
    Serial.print(t, 1);    
  }
  Serial.println("]}");
  
  }

  delay(2);
}
