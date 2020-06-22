#include "light_intensity.h"

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

//Led strip
#define LED_STRIP_PIN 10

// Photocell state
int lightIntensity = 0;

// Function for getting light intensity data
void getAndSendLightIntensityData()
{
  Serial.println("Collecting light intensity data.");
  
  //grab the current state of the photocell
  lightIntensity = analogRead(PHOTOCELL_PIN);

  //If light intensity is below 400, turn on the led strip
  if(lightIntensity < 400)
  {
    digitalWrite(LED_STRIP_PIN, HIGH);
  }

  //If light intensity is above 600, turn off the led strip
  if(lightIntensity > 600)
  {
    digitalWrite(LED_STRIP_PIN, LOW);
  }

  // Print data in serial monitor
  Serial.print("Light intensity: ");  
  Serial.println(lightIntensity);
  Serial.println("---------------------------------------------");
//  tb.sendTelemetryFloat("light intensity", lightIntensity);
}
