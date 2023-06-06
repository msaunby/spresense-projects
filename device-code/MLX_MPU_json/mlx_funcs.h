#include <Adafruit_MLX90640.h>


/*
 * Camera
 */
Adafruit_MLX90640 mlx1, mlx2;
float frame1[32*24]; // buffer for full frame of temperatures
float frame2[32*24]; // buffer for full frame of temperatures


void mlx_setup() {
    if (! mlx1.begin(MLX90640_I2CADDR_DEFAULT-1, &Wire1)) {
    Serial.println("First MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found first MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx1.serialNumber[0], HEX);
  Serial.print(mlx1.serialNumber[1], HEX);
  Serial.println(mlx1.serialNumber[2], HEX);

  if (! mlx2.begin(MLX90640_I2CADDR_DEFAULT, &Wire1)) {
    Serial.println("Second MLX90640 not found!");
    //while (1) delay(10);
  }
  Serial.println("Found second MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx2.serialNumber[0], HEX);
  Serial.print(mlx2.serialNumber[1], HEX);
  Serial.println(mlx2.serialNumber[2], HEX);
  
  mlx1.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx1.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx1.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res1 = mlx1.getResolution();
  switch (res1) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  //mlx1.setRefreshRate(MLX90640_2_HZ);
  mlx1.setRefreshRate(MLX90640_4_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx1.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }

  mlx2.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx2.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx2.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res2 = mlx2.getResolution();
  switch (res2) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  //mlx1.setRefreshRate(MLX90640_2_HZ);
  mlx2.setRefreshRate(MLX90640_4_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate2 = mlx2.getRefreshRate();
  switch (rate2) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }

}