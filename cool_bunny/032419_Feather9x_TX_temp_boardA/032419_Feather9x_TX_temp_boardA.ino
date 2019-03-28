/*
  Name: Emily Lin
  Date: 03/24/19

  Notes:
  - Code combines Board A, which reads the temperature + humidity, with the Feather9X TX Example Code
  - Sleepydog library is added to allow feather to sleep (but it is commented out because of the usb disconnecting when put to sleep)
  - If temperature is over 75 F, then data is transmitted
  - using radio.sleep

  Feather 32uF - Board A
  Board A - temperature sensor + solar panel
  SI7021 12C Temperature & Humidity Sensor

  Credits:
  Code: Using the Feather 32uF
  Code edited from: https://github.com/LowPowerLab/SI7021/blob/master/examples/readsensor/readsensor.ino
  Author: Felix Rusu

  Adafruit Watchdog Library Sleep Example
  Code: Simple example of how to do low power sleep with the watchdog timer.
  Code edited from: https://github.com/adafruit/Adafruit_SleepyDog/blob/master/examples/Sleep/Sleep.ino
  Author: Tony DiCola

*/


// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h> // for temperature sensor
#include <RH_RF95.h>  // for feather
#include <Wire.h> // for temperature sensor
#include <SI7021.h>  // for temperature sensor
#include <Adafruit_SleepyDog.h> // for feather sleep

SI7021 sensor;

//variables
int reply;

// for feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7


// define for various types of boards
#if defined(ESP8266)
/* for ESP w/featherwing */
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"

#elif defined(ESP32)
/* ESP32 feather w/wing */
#define RFM95_RST     27   // "A"
#define RFM95_CS      33   // "B"
#define RFM95_INT     12   //  next to A

#elif defined(NRF52)
/* nRF52832 feather w/wing */
#define RFM95_RST     7   // "A"
#define RFM95_CS      11   // "B"
#define RFM95_INT     31   // "C"

#elif defined(TEENSYDUINO)
/* Teensy 3.x w/wing */
#define RFM95_RST     9   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     4    // "C"
#endif


// Change to 434.0 or other frequency, must match RX's freq!
// ***Using 434.0 frequency!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //baud rate (number of bits per second) is larger
  Serial.begin(115200);

  /*
    while (!Serial) {
    delay(1);
    }
  */
  sensor.begin();

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {

  /*
    // Sleepy Dog Code : only to be used when in our home for good
    Serial.println("Going to sleep in one second...");
    delay(1000);

    // To enter low power sleep mode call Watchdog.sleep() like below
    // and the watchdog will allow low power sleep for as long as possible.
    // The actual amount of time spent in sleep will be returned (in
    // milliseconds).
    digitalWrite(13, LOW); // Show we're asleep
    int sleepMS = Watchdog.sleep(5000);

    Serial.println("I'm awake!");
  */

  // to have it sleep when not transmitting data. does not receive any data until it needs to transmit. saves 2mA
  rf95.sleep();

  // Step 1...read the data from temperature + humidity sensor

  // this driver should work for SI7020 and SI7021, this returns 20 or 21
  int deviceid = sensor.getDeviceId();
  Serial.print("device id: ");
  Serial.println(deviceid);
  delay(500);

  // get humidity and temperature in one shot, saves power because sensor takes temperature when doing humidity anyway
  si7021_env data = sensor.getHumidityAndTemperature();
  int temperature = data.celsiusHundredths / 100;
  Serial.print("temperature in C: ");
  Serial.println(temperature);

  int fahrenheit = (temperature * 1.8) + 32;
  Serial.print("temperature in F: ");
  Serial.println(fahrenheit);

  int humidity = data.humidityBasisPoints / 100;
  Serial.print("data humidity basis points: ");
  Serial.println(humidity);

  // Step 2...transmit data via rf
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting..."); // Send a message to rf95_server

  //formatting data to be transmitted
  char radiopacket[20];  //definining the transmitted string

  char str_1[3] = "T:";  //define string 1 as parameter
  char str_2[3] = "H:";  //define string 2 as parameter

  sprintf(radiopacket, "%s %i | %s %i", str_1, fahrenheit, str_2, humidity);  //concatonate parameters into single string

  //if fahrenheit is > 65 then send the data out
  if (fahrenheit >= 65) {

    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[19] = 0;  //0 register at the end of the string (?)

    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)radiopacket, 20);

    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply...");
    if (rf95.waitAvailableTimeout(1000))
    {
      // Should be a reply message for us now
      if (rf95.recv(buf, &len))
      {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      Serial.println("No reply, is there a listener around?");
    }
  }
  rf95.sleep();
}
