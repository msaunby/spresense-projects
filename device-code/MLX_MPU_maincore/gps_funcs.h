
#include <RTC.h>
/* include the GNSS library */
#include <GNSS.h>

#define STRING_BUFFER_SIZE  128       /**< %Buffer size */

#define RESTART_CYCLE       (60 * 5)  /**< positioning test term */

static SpGnss Gnss;                   /**< SpGnss object */

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

void gps_setup() {
  int error_flag = 0;

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

}

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
    Serial.print(pNavData->latitude,9);
    Serial.print(",\"lon\":");
    Serial.print(pNavData->longitude,9);
    Serial.print(",\"alt\":");
    Serial.print(pNavData->altitude,9);
    Serial.print(",\"vel\":");
    Serial.print(pNavData->velocity,9);
    Serial.print(",\"dir\":");
    Serial.print(pNavData->direction,9);
  }
  Serial.println("}");
}


char StringBuffer[1024];

static char *write_pos(SpNavData *pNavData)
{
  snprintf(StringBuffer, 1024, 
  "{\"date\":\"%04d/%02d/%02d\",\"time\":\"%02d:%02d:%02d.%06ld\",\"numSat\":%d,\"lat\":%lf,\"lon\":%lf,\"alt\":%lf,\"vel\":%lf,\"dir\":%lf,\"pdop\":%lf,\"hdop\":%lf,\"vdop\":%lf,\"tdop\":%lf}", 
    pNavData->time.year, pNavData->time.month, pNavData->time.day, 
    pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec,
    pNavData->numSatellites,
    pNavData->latitude,
    pNavData->longitude,
    pNavData->altitude,
    pNavData->velocity,
    pNavData->direction,
    pNavData->pdop,
    pNavData->hdop,
    pNavData->vdop,
    pNavData->tdop
  );
  return(StringBuffer);
}

static void print_time() {
  char StringBuffer[STRING_BUFFER_SIZE];
  RtcTime now = RTC.getTime();
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "{\"seconds\":%02d.%09ld}", now.second(), now.nsec());
  Serial.println(StringBuffer);
}

static void printGps() {
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
  print_time();
}

static char *writeGps() {
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
    return (write_pos(&NavData));
  }
  else{
    return "";
  }
}