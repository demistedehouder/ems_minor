#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

// Include necessary libraries
#include <ThingsBoard.h>
#include <Arduino.h>

// Create MQ135 class with functions
class MQ135 
{
  private:
    byte pin;

    int mqSensorValue = 0;

  public:
    MQ135(byte pin);

    void init();

    void getAirQualityData(int mqSensor, ThingsBoard tb);
};

#endif
