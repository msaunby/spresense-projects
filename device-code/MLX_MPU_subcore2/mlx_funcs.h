#include <Adafruit_MLX90640.h>

#define MLX_LOG 0
/*
 * Camera
 */
Adafruit_MLX90640 mlx1, mlx2;
float frame1[32*24]; // buffer for full frame of temperatures
float frame2[32*24]; // buffer for full frame of temperatures


void mlx_setup() {
    if (! mlx1.begin(MLX90640_I2CADDR_DEFAULT-1, &Wire1)) {
    // Serial.println("First MLX90640 not found!");
    while (1) delay(10);
  }
#if MLX_LOG
  Serial.println("Found first MLX90640");
  Serial.print("Serial number: ");
  Serial.print(mlx1.serialNumber[0], HEX);
  Serial.print(mlx1.serialNumber[1], HEX);
  Serial.println(mlx1.serialNumber[2], HEX);
#endif
  if (! mlx2.begin(MLX90640_I2CADDR_DEFAULT, &Wire1)) {
    // Serial.println("Second MLX90640 not found!");
    while (1) delay(10);
  }
#if MLX_LOG
  Serial.println("Found second MLX90640");
  Serial.print("Serial number: ");
  Serial.print(mlx2.serialNumber[0], HEX);
  Serial.print(mlx2.serialNumber[1], HEX);
  Serial.println(mlx2.serialNumber[2], HEX);
#endif

  mlx1.setMode(MLX90640_CHESS);
  mlx1.setResolution(MLX90640_ADC_18BIT);

  //mlx1.setRefreshRate(MLX90640_4_HZ);
  mlx1.setRefreshRate(MLX90640_8_HZ);

  mlx2.setMode(MLX90640_CHESS);
  mlx2.setResolution(MLX90640_ADC_18BIT);

  //mlx1.setRefreshRate(MLX90640_4_HZ);
  mlx2.setRefreshRate(MLX90640_8_HZ);

}