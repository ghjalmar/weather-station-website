/* 
 *  Sketch for sending sensor readings between Arduino and RPi
 *  Code for the Arduino (sender)
 *  Based on example code by TMRh20 and Maniacbug (communications) 
 *  and Example sketch from the SparkFunBME280 library (sensor
 */

// Sensor
#include <Wire.h>
#include "SparkFunBME280.h"

// Defining global variables
// Global sensor object
BME280 mySensor;
float sensor_readings[4];
float *sensor_reading;
float unique_val = -10000; // Sensors should never read -1000
float j;
  
// Communications
#include <SPI.h>
#include "RF24.h"
#include "printf.h"
/* nRF24L01 connected to Arduino on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
// Radio pipe addresses for the 2 nodes to communicate.
byte addresses[][6] = {"1Node","2Node"};


void setup() {
  Serial.begin(115200);

  // Setup the sensor
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x77; // I2C communications
  mySensor.settings.runMode = 3; // Normal run mode
  mySensor.settings.tStandby = 0; // Standby time
  mySensor.settings.filter = 0; // number of FIR coefficients
  mySensor.settings.tempOverSample = 1;
  mySensor.settings.pressOverSample = 1;
  mySensor.settings.humidOverSample = 1;
  // Start the sensor
  delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Wire.begin();
  Serial.print("Starting BME280... result of .begin(): 0x");
  //Calling .begin() causes the settings to be loaded
  Serial.println(mySensor.begin(), HEX);

  
  // Set up the nRF24l01 module
  printf_begin();
  Serial.println("Initializing Arduino code"); // Debugging
  // Start the nRF24 object
  radio.begin();
  
  radio.setPALevel(RF24_PA_HIGH); // Set to low due to close proximity
  radio.openWritingPipe(addresses[0]); // opening the pipes
  radio.openReadingPipe(1,addresses[1]);
  radio.startListening(); // Start the radio listening for data
  radio.printDetails(); // Debugging
  Serial.println("Details printed");
  
}

void loop() {
  // Begin with reading sensor data and then send it
  
  // Reading sensor data
  sensor_readings[0] = mySensor.readTempC(); // temp in C
  sensor_readings[1] = mySensor.readFloatPressure(); // pressure in kPa
  sensor_readings[2] = mySensor.readFloatAltitudeMeters(); // alt. in m
  sensor_readings[3] = mySensor.readFloatHumidity(); // humidity in %

  // Communications
  // Communicate by first sending a unique value which lets the RPi know that the sensor readings are going to be transmitted now
  // A total of five transmissions: 1) unique value 2) temp 3) pressure 4) altitude 5) humidity
  for (int i=0; i <= 3; i++){
    sensor_reading = &sensor_readings[i];
    // Sends -1000, -1001, -1002, -1003 and corresponding sensor reading. The negative values are for making sure that the RPi logs value to the correct field.
    Serial.println("-------- Sending integer --------");
    j = (float) i;
    j = unique_val - j;
    transmit_function(&j);
    delay(10000);
    Serial.println("-------- Sending sensor readings --------");
    transmit_function(sensor_reading);
    delay(1000);
   }
  // Delay 60 seconds before reading new data and sending it
  Serial.println("-------- ALL SENSOR READINGS SENT --------");
  delay(60000);
}


void transmit_function(float *sensor_value){
  //Serial.println(uint16_t(sensor_value),HEX);
  // Function for the transmitting a floating point value (trasnmitting the sensor data)
  radio.stopListening();   // To start the sending
  // Info to send: The sensor data (input to the function)
  unsigned long start_time = micros(); 
  if (!radio.write( sensor_value, sizeof(float) )){
     Serial.println(F("failed"));
   }
      
  radio.startListening();                                    // Now, continue listening
  
  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
  
  while ( ! radio.available() ){                             // While nothing is received
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
        timeout = true;
        break;
    }      
  }
      
  if ( timeout ){                                             // Describe the results
      Serial.println(F("Failed, response timed out."));
  }else{
      unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
      radio.read( &got_time, sizeof(unsigned long) );
      unsigned long end_time = micros();
      
      // Spew it
      Serial.print(F("Sent "));
      Serial.print(*sensor_value);
      Serial.print(F(", Round-trip delay "));
      Serial.print(end_time-start_time);
      Serial.println(F(" microseconds"));
  }
}
