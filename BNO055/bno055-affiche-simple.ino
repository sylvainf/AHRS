 /*  
   Exemple de programme avec bno055 + affichage LCD Nokia
   Sortie USB au format #YPR du razor
   Teste sur Mapple avec stm32druino

   Derive de code de Limor Fried/Ladyada  for Adafruit Industries.  
   BSD license
	https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library
        https://github.com/adafruit/Adafruit_BNO055	
   */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK) PA0 (p11)
// pin 6 - Serial data out (DIN) PA1 (p10)
// pin 5 - Data/Command select (D/C) PA2 (p9)
// pin 4 - LCD chip select (CS)  PA3 (p8)
// pin 3 - LCD reset (RST)  PA4  (p7)
#if defined (__STM32F1__)
  Adafruit_PCD8544 display = Adafruit_PCD8544(PA0, PA1, PA2, PA3, PA4);
#else
  Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
#endif


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16


/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.

   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3-5V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
   2015/AUG/27  - Added calibration and system status helpers
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

Adafruit_BNO055 bno = Adafruit_BNO055();

boolean newData = false;

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
void displaySensorStatus(void)
{
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  Serial.println("");
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
 /*
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }
*/
  /* Display the individual values */
  Serial.print(",");
  Serial.print(system, DEC);
  Serial.print(",");
  Serial.print(gyro, DEC);
  Serial.print(",");
  Serial.print(accel, DEC);
  Serial.print(",");
  Serial.print(mag, DEC);

   display.print("Sys:");
  display.print(system, DEC);
  display.print(" G:");
  display.print(gyro, DEC);
  display.print(" A:");
  display.print(accel, DEC);
  display.print(" M:");
  display.print(mag, DEC);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(115200);

 delay(1000);
  Serial.println("Start....");
  Serial.flush();

 display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(40);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("Starting...");

  display.display();

  delay(1000);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  Serial.flush();

  /* Initialise the sensor */
  
  if(!bno.begin(bno.OPERATION_MODE_NDOF_FMC_OFF ))
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    Serial.flush();
    while(1);
  }
  

Serial.println("OK, sensor initialized");Serial.println("");
  Serial.flush();

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  bno.setExtCrystalUse(true);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // I'm not sure what speed the SPI runs at .. if it is 72MHz this should bring it down to 9MHz
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/


void loop(void)
{

  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  /* Display the floating point data */
  Serial.print("YPR=");
  Serial.print(event.orientation.x, 4);
  Serial.print(",");
  Serial.print(1.*event.orientation.y, 4);
  Serial.print(",");
  Serial.print(-1.*event.orientation.z, 4);


  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);


  display.clearDisplay();

  display.setCursor(0,0);
 
  display.print("Yaw: ");
  display.println(event.orientation.x,4);
  display.print("Pitch: ");
  display.println(1.*event.orientation.y,4);
  display.print("Roll: ");
  display.println(-1.*event.orientation.z,4);
 // display.display();


   displayCalStatus();
  display.display();
  //displaySensorStatus();

  /* New line for the next sample */
  Serial.println(",OK");


  /* Wait the specified delay before requesting nex data */
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

