#include <Arduino.h>
/*
 * The ESP 01S WiFi module should be put in deep sleep mode
 * once data has been sent to the server.
 * To wak from deep sleep the reset pin is used, so must
 * be connected to an Arduino digital pin.
 * In normal mode the module draws about 70mA, but in deep sleep
 * this falls to just 17uA for the module I have.
 */

/*
 * The DHT22 sensor draws 5.5uA when idle and a higher current for a very short 
 * time when readings are taken.  Nothing special needs to be done to
 * reduce power use.  Ideally it should be powered at 5V rather than 3.3V
 * so will try using the RAW (battery) supply of approx 4V.

/* 
 * The Arduino Pro Mini, with LED, draws 4.5mA when using delay() to sleep.
 * This was measured on the RAW power pin fed with 5V, when the
 * WiFi module was in deep sleep mode.
 * Entering power down mode with WDT draws 1.6mA from raw power. Most of this
 * will be the onboard power LED. 
 * 
 * For low power see
 * https://gist.github.com/boseji/d4e031aa7ec14b498a7a6a1efedf6d55
*/



#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFiEspAT.h>
#include <SoftwareSerial.h>


const char wifi_ssid[] = "TEACHMORE";
const char wifi_key[] = "TallTree1234";
IPAddress broker(192,168,1,22);



#define ESP_RX_PIN 12
#define ESP_TX_PIN 11
#define ESP_RESET_PIN 10
const byte DHT_PIN = A1;  // Digital pin connected to the DHT sensor

#define DHTTYPE DHT22 // DHT 22 (AM2302)
// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview


SoftwareSerial Serial1(ESP_TX_PIN, ESP_RX_PIN); // my RX, my TX (i.e. swapped)
#define AT_BAUD_RATE 9600

DHT_Unified dht(DHT_PIN, DHTTYPE);

#include<PubSubClient.h>

void(* resetFunc) (void) = 0; // Force Arduino to reset

//////////////////////////////////////////////////////////////////////////
// WDT Interrupt

// WDT entry Flag
volatile uint8_t f_wdt=1;

ISR(WDT_vect)
{
	if(f_wdt == 0)
	{
		f_wdt=1; // Reset the Flag
	}
	else
	{
		Serial.println(F("WDT Overrun!!!"));
	}
}

//////////////////////////////////////////////////////////////////////////
// Sleep Configuration Function
//   Also wake-up after

void enterSleep(void)
{
	WDTCSR |= _BV(WDIE); // Enable the WatchDog before we initiate sleep

	//set_sleep_mode(SLEEP_MODE_PWR_SAVE);		/* Some power Saving */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);		/* Even more Power Savings */
	sleep_enable();

	/* Now enter sleep mode. */
	sleep_mode();
	//sleep_bod_disable();	// Additionally disable the Brown out detector

	/* The program will continue from here after the WDT timeout*/
	sleep_disable(); /* First thing to do is disable sleep. */

	/* Re-enable the peripherals. */
	power_all_enable();
}


void flash(int seconds) {
  
  digitalWrite(LED_BUILTIN, HIGH);
  
  for(int i=0; i < seconds * 4; i++){
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}


const long SLEEP_INTERVAL = 30000;

WiFiClient wifiClient;

int port = 1883;
const char topic[]  = "devices/experimental-sensor/messages/events/";
const char device_id[] = "experimental-sensor";

PubSubClient client;

void reconnect() {

 if (WiFi.begin(wifi_ssid, wifi_key) != WL_CONNECTED) {
    WiFi.deepSleep(); // Hopefully deep sleep and a wakeup will fix the problem.
    resetFunc();
 }

  for(int i = 0; i < 5; i++){
    f_wdt = 0;
    client.connect(device_id);
    if(client.connected()) break;
    f_wdt = 0;
    flash(2);
  }
  if (!client.connected()) {
    f_wdt = 0;
    Serial.println("reconnect failed");
    WiFi.deepSleep(); // Hopefully deep sleep and a wakeup will fix the problem.
    resetFunc();
  }else{
    client.publish(topic, "{}");
  }
}


int cycles;

void setup() {
  // put your setup code here, to run once:

  cycles = 0;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  /*** Setup the WDT ***/
	cli();
	/* Clear the reset flag. */
	MCUSR &= ~(1<<WDRF);

	/* In order to change WDE or the prescaler, we need to
	* set WDCE (This will allow updates for 4 clock cycles).
	*/
	WDTCSR |= (1<<WDCE) | (1<<WDE);

	/* set new watchdog timeout prescaler value */
	//WDTCSR = 1<<WDP1 | 1<<WDP2;             /* 1.0 seconds */
  //WDTCSR = 1<<WDP0 | 1<<WDP1 | 1<<WDP2; /* 2.0 seconds */
  WDTCSR = 1<<WDP3;                     /* 4.0 seconds */
  //WDTCSR = 1<<WDP0 | 1<<WDP3;           /* 8.0 seconds */

	/* Enable the WD interrupt (note no reset). */
	// WDTCSR |= _BV(WDIE); // Not here but when we go to Sleep
	sei();



  //analogReference(INTERNAL); // Use internal 1.1V reference
  analogReference(DEFAULT); // 3.3V reference

  // Enable communication
  Serial.begin(9600);

  // Initialise DHT22 temp and humidity sensor.
  digitalWrite(LED_BUILTIN, HIGH);
  dht.begin();

  Serial1.begin(AT_BAUD_RATE);

  WiFi.init(Serial1, ESP_RESET_PIN);
  // Wait a second
  delay(1000L);  

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println();
    Serial.println("Communication with WiFi module failed!");
    // Wait a second
    delay(1000L); 
    resetFunc();
  }

  while (WiFi.begin(wifi_ssid, wifi_key) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(2000L);
  }

  // Wait a second
  delay(1000L);
  client.setClient(wifiClient);
  client.setServer(broker, port);
  client.setCallback(callback);
  client.setKeepAlive( 120 ); // Default is 15 seconds.

  reconnect();
  digitalWrite(LED_BUILTIN, LOW);
}

int analogPin = A0;

void loop() {

  String msg;

  // Read temperature
  f_wdt = 0;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  // Will read again later.  This read is in the
  // hope that it will get the interface synced
  // so we don't get bad data.
  
  f_wdt = 0;
  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    reconnect();
    digitalWrite(LED_BUILTIN, LOW);
  }
 
  // Read voltage
  f_wdt = 0;
  float voltage;
  float ratio = 2.0 * 3.3;
  voltage = analogRead(analogPin);
  voltage = (voltage/1023.0) * ratio;
  msg = String("{\"kind\":\"battery\",\"value\":") + String(voltage, 2) + String("}");
  client.publish(topic,msg.c_str());
  Serial.println(msg.c_str());
  msg = String("{\"kind\":\"cycles\",\"value\":") + String(cycles) + String("}");
  client.publish(topic,msg.c_str());
  Serial.println(msg.c_str());

  // Read temperature
  f_wdt = 0;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature))
  {
    msg = String("{\"kind\":\"temperature\",\"value\":") + String(event.temperature,1) + String("}");
    client.publish(topic,msg.c_str());
  }
  Serial.println("Temperature: " + String(event.temperature, 1) + "°C");
  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity))
  {
    msg = String("{\"kind\":\"humidity\",\"value\":") + String(event.relative_humidity,1) + String("}");
    client.publish(topic,msg.c_str());
  }
  
  // Again
  f_wdt = 0;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature))
  {
    msg = String("{\"kind\":\"temperature\",\"value\":") + String(event.temperature,1) + String("}");
    client.publish(topic,msg.c_str());
  }
  Serial.println("Temperature: " + String(event.temperature, 1) + "°C");
  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity))
  {
    msg = String("{\"kind\":\"humidity\",\"value\":") + String(event.relative_humidity,1) + String("}");
    client.publish(topic,msg.c_str());
  }
  
  f_wdt = 0;
  // Going to be offline for a while, so disconnect.
  // Should also set a will.
  client.disconnect();
  delay(500L);
  //wdt_reset();
  f_wdt = 0;
  //Serial.println("putting ESP to deep sleep...");
  //f_wdt = 0;
  //delay(500L);
  f_wdt = 0;
  WiFi.deepSleep();
  for(int i=0; i < (120/4); i++){
    f_wdt = 0;
    enterSleep(); // 4 seconds
  }
  Serial.println("woke up");
  f_wdt = 0;
  WiFi.reset(ESP_RESET_PIN);
  delay(500L);
  cycles++;
}