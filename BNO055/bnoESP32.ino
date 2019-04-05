/* Exemple d'utilisation du BNO055 avec la lib d'adafruit
 Version pour ESP32 avec support Bluetooth classique
 Sortie YPR sur USB (serie) et Bluetooth simultanee

Bibliotheque bno055 pour le framework Arduino par Limor Fried/Ladyada Adafruit Industries.
        https://github.com/adafruit/Adafruit_BNO055
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "BluetoothSerial.h"



BluetoothSerial SerialBT;

Adafruit_BNO055 bno = Adafruit_BNO055(50);
 
void setup(void) 
{
  Serial.begin(115200);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  SerialBT.begin("ESP32BNO055");
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(false);
}
 
void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  Serial.print("YPR=");
  Serial.print(event.orientation.x, 1);
  Serial.print(",");
  Serial.print(event.orientation.y, 1);
  Serial.print(",");
  Serial.print(event.orientation.z, 1);
  Serial.print(",");
  if (SerialBT.hasClient()){
    SerialBT.print("YPR=");
    SerialBT.print(event.orientation.x,1);
    SerialBT.write(',');
    SerialBT.print(event.orientation.y,1);
    SerialBT.write(',');
    SerialBT.print(event.orientation.z,1); 
    SerialBT.print(",");
  }
  displayCalStatus();
  delay(20);
}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
 
  /* The data should be ignored until the system calibration is > 0 */
  if (!system)
  {
    Serial.print("! ");
  }
 
  /* Display the individual values */
  Serial.print(system, DEC);
  Serial.print(gyro, DEC);
  Serial.print(accel, DEC);
  Serial.print(mag, DEC);
  if (SerialBT.hasClient()){
    Serial.print("C");
    SerialBT.print(system, DEC);
    SerialBT.print(gyro, DEC);
    SerialBT.print(accel, DEC);
    SerialBT.println(mag, DEC);
    delay(10);

  }
  Serial.println();
}
