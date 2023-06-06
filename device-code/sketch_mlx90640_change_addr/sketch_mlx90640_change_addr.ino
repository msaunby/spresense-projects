#include <Adafruit_MLX90640.h>

Adafruit_MLX90640 mlx;

#define MLX_EEPROM_ADDR_I2C_ADDR 0x240F
#define MLX_REGISTER_ADDR_I2C_ADDR 0x8010

void setup() {

  Serial.begin(1000000);
  delay(100);

  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);

  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }
  uint16_t controlRegister1;
  int resolutionRAM;
  mlx.MLX90640_I2CRead(0, 0x800D, 1,  &controlRegister1);
  resolutionRAM = (controlRegister1 & 0x0C00) >> 10;
  printf("Res RAM %d\n", resolutionRAM);
  printf("MLX90640 Change addr\n");

  uint16_t addrRegister;

  //mlx.MLX90640_I2CWrite(0, MLX_EEPROM_ADDR_I2C_ADDR, 0);

  //  mlx.MLX90640_I2CRead(0, MLX_REGISTER_ADDR_I2C_ADDR, 1,  &addrRegister);
  mlx.MLX90640_I2CRead(0, MLX_EEPROM_ADDR_I2C_ADDR, 1,  &addrRegister);
  printf("EEPROM Addr %0x\n", addrRegister);

  mlx.MLX90640_I2CWrite(0, MLX_EEPROM_ADDR_I2C_ADDR, 0xBE32);

  //  mlx.MLX90640_I2CRead(0, MLX_REGISTER_ADDR_I2C_ADDR, 1,  &addrRegister);
  mlx.MLX90640_I2CRead(0, MLX_EEPROM_ADDR_I2C_ADDR, 1,  &addrRegister);
  printf("EEPROM Addr %0x\n", addrRegister);

  mlx.MLX90640_I2CRead(0, MLX_REGISTER_ADDR_I2C_ADDR, 1,  &addrRegister);
  printf("Register Addr %0x\n", addrRegister);
}

void loop() {
  
}
