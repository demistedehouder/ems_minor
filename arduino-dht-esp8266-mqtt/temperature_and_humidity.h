#ifndef TEMPERATURE_AND_HUMIDITY_H
#define TEMPERATURE_AND_HUMIDITY_H// DHT

// Include necessary libraries
#include <ThingsBoard.h>
#include <Arduino.h>
#include "DHT.h"

// Create DHT11 object class with functions
class DHT11Object 
{
  private:
    byte pin;
    
  public:
    DHT11Object(byte pin);

    void init();

    // Function takes parameters from main file
    void getAndSendTemperatureAndHumidityData(int fanPin, ThingsBoard tb, DHT dht);
};

#endif
