#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <ThingsBoard.h>
#include <Servo.h>

// Wifi initialization
#define WIFI_AP "BZiggoA3CF7F4"
#define WIFI_PASSWORD "rPkcepn38cQm"

// Get thingsboard token
#define TOKEN "nulIlMPhQBOzziPdqUoq"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

//Ground humidity sensor
#define GHUMIDITY_PIN A1

//Fan 
#define FAN_PIN 13

//Led strip
#define LED_STRIP_PIN 10

// Photocell state
int lightIntensity = 0;

// Ground humidity value
float gHumidity = 0;

// Digital pin 8 will be used as indicator for the MQ-135 sensor
int MQ_SENSOR_PIN = 8;

// Analog pin 2 will be used for MQ-135 sensor
int mqSensor = A2;

// Initial value of sensor set to 0
int mqSensorValue = 0;

// Create servo object
Servo windowServo;

// Set IP for server
char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// Initialize ESP client
ThingsBoard tb(espClient);

SoftwareSerial soft(2, 3); // RX, TX

// Wifi vars
int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  dht.begin();
  InitWiFi();
  lastSend = 0;
  pinMode(FAN_PIN, OUTPUT);
  pinMode(MQ_SENSOR_PIN, OUTPUT);
  windowServo.attach(5);
}

void loop() {
  
  delay(1000);
  status = WiFi.status();
  
  // While wifi is not yet connected, try to connect
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !tb.connected() ) {
    reconnect();
  }
  
  // Update and send only after 1 seconds
  if ( millis() - lastSend > 1000 ) { 
    getAndSendTemperatureAndHumidityData();
    getAndSendLightIntensityData();
    getAndSendGroundHumidityData();
    getAirQualityData();
    lastSend = millis();
  }
  tb.loop();
}

// Function for getting quality of the air using MQ-135 sensor
void getAirQualityData()
{
  // Read input on analog pin 2
  mqSensorValue = analogRead(mqSensor);
  // Divide value by 10 to translate output to %
  mqSensorValue = mqSensorValue/10;

  // If value gets above 50%, blink the indicator led
  if(mqSensorValue > 50)
  {
    digitalWrite(MQ_SENSOR_PIN, HIGH);
  }
  // If value is below 50, don't blink the indicator led
  else
  {
    digitalWrite(MQ_SENSOR_PIN, LOW);
  } 

  // Print data in serial monitor
  Serial.println("Collecting air quality data.");
  Serial.print("Conductivity: ");  
  Serial.print( mqSensorValue);
  Serial.println("%");
  Serial.println("---------------------------------------------");
  
  // Send data to thingsboard where it can be displayed in a chart
  tb.sendTelemetryFloat("air quality", mqSensorValue);
}

// Function for getting ground humidity
void getAndSendGroundHumidityData()
{
  //loop through output 100 times at a low delay
  for (int i = 0; i <= 100; i++)
  {
    gHumidity = gHumidity + analogRead(GHUMIDITY_PIN);
    delay(1);
  }

  //devide output bij 100 to get the average humidity
  gHumidity = gHumidity/100.0;

  // Print data in serial monitor
  Serial.println("Collecting ground humidity data.");
  Serial.print("Ground Humidity: ");  
  Serial.println(gHumidity);
  Serial.println("---------------------------------------------");
  //send data to thingsboard where it can be displayed in a chart
  tb.sendTelemetryFloat("ground humidity", gHumidity);
}

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

  // If temperature goes above 24 degrees, fan turns on and servo rotates
  if(temperature > 24)
  {
    digitalWrite(FAN_PIN, HIGH);
    //windowServo.write(90); 
  }

  // If temperature goes below 22 degrees, fan turns off and servo rotates
  if(temperature < 22)
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
  Serial.print("Servo state: ");
  Serial.println("---------------------------------------------");

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);
}

void getAndSendLightIntensityData()
{
  Serial.println("Collecting light intensity data.");
  
  //grab the current state of the photocell
  lightIntensity = analogRead(PHOTOCELL_PIN);

  //If light intensity is below 400, turn on the led strip
  if(lightIntensity < 400)
  {
    digitalWrite(LED_STRIP_PIN, HIGH);
  }

  //If light intensity is above 600, turn off the led strip
  if(lightIntensity > 600)
  {
    digitalWrite(LED_STRIP_PIN, LOW);
  }

  // Print data in serial monitor
  Serial.print("Light intensity: ");  
  Serial.println(lightIntensity);
  Serial.println("---------------------------------------------");
  tb.sendTelemetryFloat("light intensity", lightIntensity);
}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
