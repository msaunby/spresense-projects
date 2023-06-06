// Two cameras

#include <Adafruit_MLX90640.h>

#define USE_GPS 1

#include <RTC.h>
#if USE_GPS
/* include the GNSS library */
#include <GNSS.h>

#define STRING_BUFFER_SIZE  128       /**< %Buffer size */

#define RESTART_CYCLE       (60 * 5)  /**< positioning test term */

static SpGnss Gnss;                   /**< SpGnss object */

/**
 * @enum ParamSat
 * @brief Satellite system
 */

#if 0
enum ParamSat {
  eSatGps,            /**< GPS                     World wide coverage  */
  eSatGlonass,        /**< GLONASS                 World wide coverage  */
  eSatGpsSbas,        /**< GPS+SBAS                North America        */
  eSatGpsGlonass,     /**< GPS+Glonass             World wide coverage  */
  eSatGpsBeidou,      /**< GPS+BeiDou              World wide coverage  */
  eSatGpsGalileo,     /**< GPS+Galileo             World wide coverage  */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA           East Asia & Oceania  */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsBeidouQz1c,  /**< GPS+BeiDou+QZSS_L1CA    East Asia & Oceania  */
  eSatGpsGalileoQz1c, /**< GPS+Galileo+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S  Japan                */
};
#endif

/* Set this parameter depending on your current region. */
// static enum ParamSat satType =  eSatGps;


/**
 * @brief Turn on / off the LED0 for CPU active notification.
 */
static void Led_isActive(void)
{
  static int state = 1;
  if (state == 1)
  {
    ledOn(PIN_LED0);
    state = 0;
  }
  else
  {
    ledOff(PIN_LED0);
    state = 1;
  }
}

/**
 * @brief Turn on / off the LED1 for positioning state notification.
 *
 * @param [in] state Positioning state
 */
static void Led_isPosfix(bool state)
{
  if (state)
  {
    ledOn(PIN_LED1);
  }
  else
  {
    ledOff(PIN_LED1);
  }
}

/**
 * @brief Turn on / off the LED3 for error notification.
 *
 * @param [in] state Error state
 */
static void Led_isError(bool state)
{
  if (state)
  {
    ledOn(PIN_LED3);
  }
  else
  {
    ledOff(PIN_LED3);
  }
}

#endif
// USE_GPS

Adafruit_MLX90640 mlx1, mlx2;
float frame1[32*24]; // buffer for full frame of temperatures
float frame2[32*24]; // buffer for full frame of temperatures

void setup() {
  int error_flag = 0;

  Serial.begin(1000000);
  /* Initialize RTC */
  RTC.begin();  

   /* Wait HW initialization done. */
  sleep(3);

  /* Turn on all LED:Setup start. */
  ledOn(PIN_LED0);
  ledOn(PIN_LED1);
  ledOn(PIN_LED2);
  ledOn(PIN_LED3);

  /* Set Debug mode to Info */
  Gnss.setDebugMode(PrintInfo);

  int result;

  /* Activate GNSS device */
  result = Gnss.begin();

  if (result != 0)
  {
    Serial.println("Gnss begin error!!");
    error_flag = 1;
  }
  else
  {
    /* Setup GNSS
     *  It is possible to setup up to two GNSS satellites systems.
     *  Depending on your location you can improve your accuracy by selecting different GNSS system than the GPS system.
     *  See: https://developer.sony.com/develop/spresense/developer-tools/get-started-using-nuttx/nuttx-developer-guide#_gnss
     *  for detailed information.
    */

    Gnss.select(GPS);
    Gnss.select(GALILEO);
    //Gnss.select(GLONASS);

    /* Start positioning */
    result = Gnss.start(HOT_START);
    if (result != 0)
    {
      Serial.println("Gnss start error!!");
      error_flag = 1;
    }
    else
    {
      Serial.println("Gnss setup OK");
    }
  }

  /* Start 1PPS output to PIN_D02 
   * See https://en.wikipedia.org/wiki/Pulse-per-second_signal
   */
  //Gnss.start1PPS();

  /* Turn off all LED:Setup done. */
  ledOff(PIN_LED0);
  ledOff(PIN_LED1);
  ledOff(PIN_LED2);
  ledOff(PIN_LED3);

  /* Set error LED. */
  if (error_flag == 1)
  {
    Led_isError(true);
    exit(0);
  }

  // See also Wire1

  if (! mlx1.begin(MLX90640_I2CADDR_DEFAULT-1, &Wire)) {
    Serial.println("First MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found first MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx1.serialNumber[0], HEX);
  Serial.print(mlx1.serialNumber[1], HEX);
  Serial.println(mlx1.serialNumber[2], HEX);

  if (! mlx2.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("Second MLX90640 not found!");
    while (1) delay(10);
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

  // Maximum speed on default I2C interface is 400kHz
   Wire.setClock(TWI_FREQ_400KHZ);
   Wire1.setClock(TWI_FREQ_400KHZ);

}



/**
 * @brief %Print position information.
 */
static void print_pos(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];

  Serial.print("{");
  /* print time */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "\"date\":\"%04d/%02d/%02d\"", pNavData->time.year, pNavData->time.month, pNavData->time.day);
  Serial.print(StringBuffer);
  Serial.print(",");
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "\"time\":\"%02d:%02d:%02d.%06ld\"", pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
  Serial.print(StringBuffer);
  Serial.print(",");
  /* print satellites count */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "\"numSat\":%d", pNavData->numSatellites);
  Serial.print(StringBuffer);
  Serial.print(",");

  /* print position data */
  if (pNavData->posFixMode == FixInvalid)
  {
    Serial.print("\"fix\":0,");
  }
  else
  {
    Serial.print("\"fix\":1,");
  }
  //if (pNavData->posDataExist == 0)
  //{
  //  Serial.print("No Position");
  //}
  //else
  {
    Serial.print("\"lat\":");
    Serial.print(pNavData->latitude, 6);
    Serial.print(",\"lon\":");
    Serial.print(pNavData->longitude, 6);
  }
  Serial.println("}");
}

/**
 * @brief %Print satellite condition.
 */
static void print_condition(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];
  unsigned long cnt;

  /* Print satellite count. */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSatellites:%2d\n", pNavData->numSatellites);
  Serial.print(StringBuffer);

  for (cnt = 0; cnt < pNavData->numSatellites; cnt++)
  {
    const char *pType = "---";
    SpSatelliteType sattype = pNavData->getSatelliteType(cnt);

    /* Get satellite type. */
    /* Keep it to three letters. */
    switch (sattype)
    {
      case GPS:
        pType = "GPS";
        break;

      case GLONASS:
        pType = "GLN";
        break;

      case QZ_L1CA:
        pType = "QCA";
        break;

      case SBAS:
        pType = "SBA";
        break;

      case QZ_L1S:
        pType = "Q1S";
        break;

      case BEIDOU:
        pType = "BDS";
        break;

      case GALILEO:
        pType = "GAL";
        break;

      default:
        pType = "UKN";
        break;
    }

    /* Get print conditions. */
    unsigned long Id  = pNavData->getSatelliteId(cnt);
    unsigned long Elv = pNavData->getSatelliteElevation(cnt);
    unsigned long Azm = pNavData->getSatelliteAzimuth(cnt);
    float sigLevel = pNavData->getSatelliteSignalLevel(cnt);

    /* Print satellite condition. */
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "[%2ld] Type:%s, Id:%2ld, Elv:%2ld, Azm:%3ld, CN0:", cnt, pType, Id, Elv, Azm );
    Serial.print(StringBuffer);
    Serial.println(sigLevel, 6);
  }
}

void printClock(RtcTime &rtc)
{
  printf("{\"rtc-date\":\"%04d/%02d/%02d\",\"rtc-time\":\"%02d:%02d:%02d\"}\n",
         rtc.year(), rtc.month(), rtc.day(),
         rtc.hour(), rtc.minute(), rtc.second());
}

void updateClock()
{
  static RtcTime old;
  RtcTime now = RTC.getTime();

  // Display only when the second is updated
  if (now != old) {
    printClock(now);
    old = now;
  }
}

#define CMD_BUF_LEN 8
char cmdBuf[CMD_BUF_LEN];
int cmdBufIdx = 0;

void loop() {

  bool getImageData = false;
  bool getGpsData = false;

  static int LastPrintMin = 0;

  while (Serial.available() > 0) {
    // Command ends with either CR (13) or NL (10)
    int inChar = Serial.read();
    if ((inChar == 13) || (inChar == 10)){
      // Discard remaining chars
      while (Serial.available() > 0) Serial.read();
      cmdBuf[cmdBufIdx] = '\0';
      cmdBufIdx = 0;
      Serial.print("COMMAND ");
      Serial.println(cmdBuf);
      if (String(cmdBuf) == String("start")){
        Serial.println("START COMMAND");
      }
      break;
    }else{
      cmdBuf[cmdBufIdx] = inChar;   
      Serial.print("*** Keys:");
      Serial.println(inChar);
      cmdBufIdx++;
      cmdBufIdx = cmdBufIdx % CMD_BUF_LEN;
    }
  }

  if (getImageData) { 
    if (mlx1.getFrame(frame1) != 0) {
      Serial.println("Failed");
      return;
    }

    if (mlx2.getFrame(frame2) != 0) {
      Serial.println("Image 2 Failed");
    }
  } else {
    delay(500);
  }

  if (getGpsData) {
  if (Gnss.isUpdate())
  {
    /* Get NaviData. */
    SpNavData NavData;
    Gnss.getNavData(&NavData);
    SpGnssTime *time = &NavData.time;

    // Check if the acquired UTC time is accurate
    if (time->year >= 2000) {
      RtcTime now = RTC.getTime();
      // Convert SpGnssTime to RtcTime
      RtcTime gps(time->year, time->month, time->day,
                  time->hour, time->minute, time->sec, time->usec * 1000);
      int diff = now - gps;
      if (abs(diff) >= 1) {
        RTC.setTime(gps);
      }
    }
    /* Print position information. */
    print_pos(&NavData);
    }
    
  }

  // Display the current time every second
  updateClock();

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
}