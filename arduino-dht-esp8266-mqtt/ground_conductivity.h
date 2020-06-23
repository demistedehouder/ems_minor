#ifndef GROUND_CONDUCTIVITY_H
#define GROUND_CONDUCTIVITY_H

// Include necessary libraries
#include <ThingsBoard.h>
#include <Arduino.h>

// Create FC_28 class with functions
class FC_28
{
  private:
    byte pin;

    int waterCounter = 0;

    int groundSensorValue = 0;

  public:
    FC_28(byte pin);
 
    void init();

    void getGroundResistanceData(int groundResistance, int waterPump, ThingsBoard tb);
};

#endif
