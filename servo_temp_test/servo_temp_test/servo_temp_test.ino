/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include "DHT.h"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(115200);
  dht.begin();
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() 
{
  delay(2000);
  getTemperature();
}

void getTemperature()
{
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature)) {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }

  // If temperature goes above 24 degrees, fan turns on and servo rotates
  if(temperature > 24)
  {
    pos = 90;
    myservo.write(pos); 
  }

  // If temperature goes below 22 degrees, fan turns off and servo rotates
  if(temperature < 22)
  {
    pos = 0;
    myservo.write(0); 
  }

  Serial.print(temperature); 
  Serial.println(" temp");
  Serial.print(pos);
  Serial.println(" pos");
}
