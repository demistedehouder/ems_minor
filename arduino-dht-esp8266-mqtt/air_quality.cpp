// Include header file
#include "air_quality.h"

// Constructor
MQ135::MQ135(byte pin)
{
  this->pin = pin;
  init();
}

// Initialization, set sensor pin to right mode
void MQ135::init() 
{
  pinMode(pin, OUTPUT);
}

// Function for getting and sending sensor value
void MQ135::getAirQualityData(int mqSensor, ThingsBoard tb)
{
  
  // Read input on analog pin 2
  mqSensorValue = analogRead(mqSensor);
  // Divide value by 10 to translate output to %
  mqSensorValue = mqSensorValue/10;

  // If value gets above 20%, turn on the indicator led
  if(mqSensorValue > 20)
  {
    digitalWrite(pin, HIGH);
  }
  // If value is below 20, don't turn on the indicator led
  else
  {
    digitalWrite(pin, LOW);
  } 

  // Print data in serial monitor
  Serial.println("Collecting air quality data.");
  Serial.print("Air Resistance: ");  
  Serial.print( mqSensorValue);
  Serial.println("%");
  Serial.println("---------------------------------------------");
  tb.sendTelemetryFloat("air resistance", mqSensorValue);
}
