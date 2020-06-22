#include "ground_conductivity.h"

//// Ground conductivity value
//float gConductivity = 0;
//
//// Counter for watering system
//int waterCounter = 0;

//Ground Conductivity sensor
#define GCONDUCTIVITY_PIN A1

//Water pump
#define WATER_PUMP 7

// Function for getting ground conductivity
void getAndSendGroundConductivityData(float gConductivity, int waterCounter)
{
  //loop through output 100 times at a low delay
  for (int i = 0; i <= 100; i++)
  {
    gConductivity = gConductivity + analogRead(GCONDUCTIVITY_PIN);
    delay(1);
  }

  //devide output by 100 to get the average conductivity
  gConductivity = gConductivity/100.0;
  digitalWrite(WATER_PUMP, LOW);

  // Print data in serial monitor
  Serial.println("Collecting ground conductivity data.");
  Serial.print("Ground Conductivity: ");
  Serial.println(gConductivity);
  Serial.print("Charging Water Pump ⚡: ");
  Serial.print(waterCounter*10);
  Serial.println("%");
  
  if(waterCounter >= 10){
    if(gConductivity >= 140){
      digitalWrite(WATER_PUMP, HIGH);
      Serial.println("Water Given 💧");
    }
    else{
      Serial.println("No Water Needed 🌊");
    }
    waterCounter = 0;
  } else{
    waterCounter++;
  }
  Serial.println("---------------------------------------------");
  //send data to thingsboard where it can be displayed in a chart
//  tb.sendTelemetryFloat("ground conductivity", gConductivity);
}
