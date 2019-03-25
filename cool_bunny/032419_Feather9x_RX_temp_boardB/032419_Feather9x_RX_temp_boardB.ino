/*
  Name: Emily Lin
  Date: 03/23/19

  Notes:
  - Code combines Board B, which receives the temperature + humidity to display on LCD Display, with the Feather9X RX Example Code
  - turns on and off fan

  Feather 32uF <--> Board B
  Board B - receives the temperature + humidity to display on LCD Display

  SI7021 12C Temperature & Humidity Sensor

  Credits:
  Code edited for Feather 32uF
  Code edited from: https://github.com/LowPowerLab/SI7021/blob/master/examples/readsensor/readsensor.ino
  By Felix Rusu
*/



// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <Adafruit_SleepyDog.h>

#include <SPI.h>
#include <RH_RF95.h>

#include <Wire.h>  //for temperature sensor
#include <LiquidCrystal.h>  //for lcd display

LiquidCrystal lcd(5, 12, 6, 9, 10, 11);   // lcd pins

int reply;

int fan = A0;  // pin for fan

int temperature;
int humidity;

#define VBATPIN A9

// for Feather32u4 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7


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
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

void setup()
{
  lcd.begin(16, 2); //number of column and rows in lcd

  pinMode(fan, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);

  /*
    while (!Serial) {
    delay(1);
    }
  */
  delay(100);

  Serial.println("Feather LoRa RX Test!");

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

void loop()
{

  //reads the battery voltage and prints as serial
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);

  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len); // printing out the raw hex data recieved
      Serial.print("Got: ");

      //taking the numbers in the data as integers to be used for other operations
      String rec_packet = String((char*)buf); // turning the incoming data (aka buf) into a string which could be used later as a variable
      temperature = rec_packet.substring(3, 5).toInt(); //read range 3-5 of string and turn string into an integer
      humidity = rec_packet.substring(11, 13).toInt(); // read range 11-13 of string and turn string into an integer

      Serial.println(rec_packet); // turns the received packet into characters (converting from hex to string)

      Serial.print("RSSI: ");  // don't understand (?)
      Serial.println(rf95.lastRssi(), DEC);

      //print string rec_packet (temp + humidity) onto lcd
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temperature: ");
      lcd.print(temperature);
      lcd.print("F");

      lcd.setCursor(0, 1);
      lcd.print("Humidity: ");
      lcd.print(humidity);
      lcd.print("%");

      if (temperature > 80) {
        digitalWrite(fan, HIGH);
        Serial.println(temperature);
      }
      else {
        digitalWrite(fan, LOW);
      }


      /*
        // Send a reply
        uint8_t data[] = "And hello back to you";
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        digitalWrite(LED, LOW);
      */
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  //  else{
  //    Serial.println("No signal received!");
  //    delay(2000);
  // }



}
