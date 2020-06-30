// Include header file
#include "temperature_and_humidity.h"

// Constructor, initialize pin and call initialization function
DHT11Object::DHT11Object(byte pin)
{
  this->pin = pin;
  init();
}

// Initialization, set fan pin to the right mode
void DHT11Object::init() 
{
  pinMode(pin, OUTPUT);
}

// Function for getting temperature and humidity data
void DHT11Object::getAndSendTemperatureAndHumidityData(int fanPin, ThingsBoard tb, DHT dht)
{  
  // Set variables to read the right data from DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // If temperature goes above 24 degrees and humidity above 50, fan turns on
  if(temperature > 24 && humidity > 50)
  {
    digitalWrite(fanPin, HIGH);
    //windowServo.write(90); 
  }

  // If temperature goes below 22 degrees and humidity below 40, fan turns off
  if(temperature < 22 && humidity < 40)
  {
    digitalWrite(fanPin, LOW);
    //windowServo.write(0); 
  }

  // Print data in serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  Serial.println("---------------------------------------------");

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);
}
