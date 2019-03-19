/*
  Using the BNO080 IMU
  By: Nathan Seidle
  SparkFun Electronics
  Date: July 27th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Sylvain : add bluetooth support

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14686

  This example shows how to use the SPI interface on the BNO080. It's fairly involved
  and requires 7 comm wires (plus 2 power), soldering the PS1 jumper, and clearing
  the I2C jumper. We recommend using the Qwiic I2C interface, but if you need speed
  SPI is the way to go.

  This example shows how to output the i/j/k/real parts of the rotation vector.
  https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation

  Hardware modifications:
  The PS1 jumper must be closed
  The PS0 jumper must be open. PS0/WAKE is connected and the WAK pin is used to bring the IC out of sleep.
  The I2C pull up jumper must be cleared/open

  Hardware Connections:
  Don't hook the BNO080 to a normal 5V Uno! Either use the Qwiic system or use a 
  microcontroller that runs at 3.3V.
  Arduino 13 = BNO080 SCK
  12 = SO
  11 = SI
  10 = !CS
  9 = WAK
  8 = !INT
  7 = !RST
  3.3V = 3V3
  GND = GND
*/

#include <SPI.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#include "SparkFun_BNO080_Arduino_Library.h"
BNO080 myIMU;

//These pins can be any GPIO
byte imuCSPin = 4;
byte imuWAKPin = 16;
byte imuINTPin = 17;
byte imuRSTPin = 5;

unsigned long startTime; //Used for calc'ing Hz
long measurements = 0; //Used for calc'ing Hz

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32BNO080");

  Serial.println();
  Serial.println("BNO080 SPI Read Example");

  myIMU.enableDebugging(Serial); //Pipe debug messages to Serial port
  
  if(myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin) == false)
  {
    Serial.println("BNO080 over SPI not detected. Are you sure you have all 6 connections? Freezing...");
    while(1);
  }

  //You can also call begin with SPI clock speed and SPI port hardware
  //myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin, 1000000);
  //myIMU.beginSPI(imuCSPin, imuWAKPin, imuINTPin, imuRSTPin, 1000000, SPI1);

  //The IMU is now connected over SPI
  //Please see the other examples for library functions that you can call

  myIMU.enableRotationVector(50); //Send data update every 50ms

  Serial.println(F("Rotation vector enabled"));
  Serial.println(F("Output in form i, j, k, real, accuracy"));

  startTime = millis();
}

void loop()
{
  //Serial.println("Doing other things");
  delay(10); //You can do many other things. We spend most of our time printing and delaying.
  
  //Look for reports from the IMU
  if (myIMU.dataAvailable() == true)
  {
    float q[4];
    q[0] = myIMU.getQuatI();
    q[1] = myIMU.getQuatJ();
    q[2] = myIMU.getQuatK();
    q[3] = myIMU.getQuatReal();
    float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();
    measurements++;


    float  yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
    float pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    float roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI; 
    yaw   -= 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
    roll  *= 180.0f / PI;
    Serial.print(yaw, 2);
    Serial.print(", ");
    Serial.print(pitch, 2);
    Serial.print(", ");
    Serial.print(roll, 2);

    Serial.print(F(","));
    Serial.print(180.0f / PI*quatRadianAccuracy, 2);
    Serial.print(F(", "));
    Serial.print((float)measurements / ((millis() - startTime) / 1000.0), 2);
    Serial.print(F("Hz"));
    Serial.println();
    if (SerialBT.hasClient()){
     SerialBT.print(yaw, 2);
     SerialBT.print(", ");
     SerialBT.print(pitch, 2);
     SerialBT.print(", ");
     SerialBT.print(roll, 2);
   }
  }
  
}
