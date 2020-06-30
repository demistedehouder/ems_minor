// Include header file
#include "ground_resistance.h"

// Constructor
FC_28::FC_28(byte pin)
{
  this->pin = pin;
  init();
}

// Initialization, set sensor pin to right mode
void FC_28::init() 
{
  pinMode(pin, OUTPUT);
}

// Function for getting ground conductivity
void FC_28::getGroundResistanceData(int groundResistance, int waterPump, ThingsBoard tb)
{
  //loop through output 100 times at a low delay
  for (int i = 0; i <= 100; i++)
  {
    groundSensorValue = groundSensorValue + analogRead(groundResistance);
  }

  //devide output by 100 to get the average conductivity
  groundSensorValue = groundSensorValue/100.0;
  digitalWrite(waterPump, LOW);

  // Print data in serial monitor
  Serial.print("Ground Resistance: ");
  Serial.println(groundSensorValue);
  Serial.print("Charging Water Pump ⚡: ");
  Serial.print(waterCounter*10);
  Serial.println("%");

  // If water counter gets above 10 and water resistance is above 140, water the plant
  if(waterCounter >= 10){
    if(groundSensorValue >= 140){
      digitalWrite(waterPump, HIGH);
      Serial.println("Water Given 💧");
    }
    else{
      Serial.println("No Water Needed 🌊");
    }
    // Set water to 0 after watering
    waterCounter = 0;
  }
  else if(waterCounter < 10){
    waterCounter++;
  }
  Serial.println("---------------------------------------------");
  tb.sendTelemetryFloat("ground resistance", groundSensorValue);
}
