// Digital pin 8 will be called 'pin8'
int mqSensorPin = 8;
// Analog pin 0 will be called 'sensor'
int mqSensor = A2;
// Set the initial sensorValue to 0
int mqSensorValue = 0;

// The setup routine runs once when you press reset
void setup() {
  // Initialize the digital pin 8 as an output
  pinMode(mqSensorPin, OUTPUT);
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
}

// Function for getting quality of the air using MQ-135 sensor
void getAirQuality()
{
  // Read input on analog pin 2
  mqSensorValue = analogRead(mqSensor);
  mqSensorValue = mqSensorValue/10;

  Serial.println(mqSensorValue, DEC);

  if(mqSensorValue > 50)
  {
    digitalWrite(mqSensorPin, HIGH);
    delay(300);
    digitalWrite(mqSensorPin, LOW);
  }
  else
  {
    digitalWrite(mqSensorPin, LOW);
  }
  
}

// The loop routine runs over and over again forever
void loop() {

  getAirQuality();
  delay(300);

}
