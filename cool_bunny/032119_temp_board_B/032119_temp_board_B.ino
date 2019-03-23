
/*
  Name: Emily Lin
  Date: 03/21/19

  Feather 32uF - Board B
  Board A - temperature sensor + solar panel
  SI7021 12C Temperature & Humidity Sensor

  Notes: Code edited for Feather 32uF
  Code edited from: https://github.com/LowPowerLab/SI7021/blob/master/examples/readsensor/readsensor.ino

  Credit: Felix Rusu
*/


#include <Wire.h>
//#include <SI7021.h>
#include <LiquidCrystal.h>

//SI7021 sensor;
/* old version with Arduino Mega
   LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
*/
LiquidCrystal lcd(5, 12, 6, 9, 10, 11);

//variables
int reply;


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop() {

  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("test");
    lcd.print(i);
    delay(5000);
  }


  //  lcd.print("Temperature: ");
  //  Serial.println("Temperature");
  ////lcd.print(temperature);
  //  lcd.print("C");


  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  //lcd.print(humidity);
  lcd.print("%");

  delay(5000);
}

/* TEMP SENSOR CODE
  // this driver should work for SI7020 and SI7021, this returns 20 or 21
  int deviceid = sensor.getDeviceId();
  Serial.print("device id: ");
  Serial.println(deviceid);
  delay(500);

  // get humidity and temperature in one shot, saves power because sensor takes temperature when doing humidity anyway
  si7021_env data = sensor.getHumidityAndTemperature();
  int temperature = data.celsiusHundredths/100;
  Serial.print("data celsius in hundreths: ");
  Serial.println(temperature);
*/
