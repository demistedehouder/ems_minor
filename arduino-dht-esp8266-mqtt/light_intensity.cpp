// Include header file
#include "light_intensity.h"

// Constructor
Photocell::Photocell(byte pin)
{
  this->pin = pin;
}

// Function for getting light intensity data
void Photocell::getAndSendLightIntensityData(int photoPin, int ledStripPin, ThingsBoard tb)
{  
  //grab the current state of the photocell
  lightValue = analogRead(photoPin);

  //If light intensity is below 400, turn on the led strip
  if(lightValue < 400)
  {
    digitalWrite(ledStripPin, HIGH);
  }

  //If light intensity is above 600, turn off the led strip
  if(lightValue > 600)
  {
    digitalWrite(ledStripPin, LOW);
  }

  // Print data in serial monitor
  Serial.print("Light intensity: ");  
  Serial.println(lightValue);
  Serial.println("---------------------------------------------");
  tb.sendTelemetryFloat("light intensity", lightValue);
}
