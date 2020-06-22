#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#include <Arduino.h>

class MQ135 
{
  private:
    byte pin;

  public:
    MQ135(byte pin);

    void init();

    int mqSensorValue = 0;

    int getAirQualityData(int mqSensor);
};

#endif
