#include "DHT.h"
#include "temperature_and_humidity.h"
// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

//Fan 
#define FAN_PIN 13

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// Function for getting temperature and humidity data
void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // If temperature goes above 24 degrees and humidity above 60, fan turns on
  if(temperature > 24 && humidity > 60)
  {
    digitalWrite(FAN_PIN, HIGH);
    //windowServo.write(90); 
  }

  // If temperature goes below 22 degrees and humidity below 50, fan turns off
  if(temperature < 22 && humidity < 50)
  {
    digitalWrite(FAN_PIN, LOW);
    //windowServo.write(0); 
  }

  // Print data in serial monitor
  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
  Serial.println("---------------------------------------------");

//  tb.sendTelemetryFloat("temperature", temperature);
//  tb.sendTelemetryFloat("humidity", humidity);
}
