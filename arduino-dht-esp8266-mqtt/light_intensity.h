#ifndef LIGHT_INTENSITY_H
#define LIGHT_INTENSITY_H

// Include necessary libraries
#include <ThingsBoard.h>
#include <Arduino.h>

// Create photocell class with functions
class Photocell 
{
  private:
    byte pin;
    
    int lightValue = 0;


  public:
    Photocell(byte pin);
    
    void getAndSendLightIntensityData(int photoPin, int ledStripPin, ThingsBoard tb); 
};

#endif
