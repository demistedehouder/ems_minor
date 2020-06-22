#include "air_quality.h"

MQ135::MQ135(byte pin)
{
  this->pin = pin;
  init();
}

void MQ135::init() 
{
  pinMode(pin, OUTPUT);
}

int mqSensorValue = 0;

int MQ135::getAirQualityData(int mqSensor)
{
  
  // Read input on analog pin 2
  mqSensorValue = analogRead(mqSensor);
  // Divide value by 10 to translate output to %
  mqSensorValue = mqSensorValue/10;

  // If value gets above 50%, blink the indicator led
  if(mqSensorValue > 50)
  {
    digitalWrite(pin, HIGH);
  }
  // If value is below 50, don't blink the indicator led
  else
  {
    digitalWrite(pin, LOW);
  } 

  // Print data in serial monitor
  Serial.println("Collecting air quality data.");
  Serial.print("Air  Conductivity: ");  
  Serial.print( mqSensorValue);
  Serial.println("%");
  Serial.println("---------------------------------------------");

  return mqSensorValue;
  
  // Send data to thingsboard where it can be displayed in a chart
//  tb.sendTelemetryFloat("air quality", mqSensorValue);
}
