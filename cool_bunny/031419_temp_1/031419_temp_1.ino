/*
  SI7021 12C Temperature & Humidity Sensor
  Name: Emily Lin
  Date: 03/14/19

  Code edited from: https://github.com/LowPowerLab/SI7021/blob/master/examples/readsensor/readsensor.ino

  Credit: Felix Rusu
*/



#include <Wire.h>
#include <SI7021.h>

SI7021 sensor;

void setup() {
  Serial.begin(9600);
  sensor.begin();
}

void loop() {


  // this driver should work for SI7020 and SI7021, this returns 20 or 21
  int deviceid = sensor.getDeviceId();
  Serial.print("device id: ");
  Serial.println(deviceid);
  delay(5000);

  //cool down
  delay(20000);

  // get humidity and temperature in one shot, saves power because sensor takes temperature when doing humidity anyway
  si7021_env data = sensor.getHumidityAndTemperature();
  int temperature = data.celsiusHundredths/100;
  Serial.print("data celsius in hundreths: ");
  Serial.println(temperature);

  int humidity = data.humidityBasisPoints;
  Serial.print("data humidity basis points: ");
  Serial.println(humidity);

  delay(5000);

}
