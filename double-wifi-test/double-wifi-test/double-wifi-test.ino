#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <ThingsBoard.h>
#include <Servo.h>

// Wifi initialization
#define WIFI_AP_1 "Ziggo685F28E"
#define WIFI_PASSWORD_1 "pn2wR8vsrxTf"

// Get thingsboard token
#define TOKEN "4oC2lJQxmBXxUmdSVpGe"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

//Ground Conductivity sensor
#define GCONDUCTIVITY_PIN A1

//Fan 
#define FAN_PIN 13

//Led strip
#define LED_STRIP_PIN 10

//Water pump
#define WATER_PUMP 7

// Photocell state
int lightIntensity = 0;

// Ground conductivity value
float gConductivity = 0;

// Digital pin 8 will be used as indicator for the MQ-135 sensor
int MQ_SENSOR_PIN = 8;

// Analog pin 2 will be used for MQ-135 sensor
int mqSensor = A2;

// Initial value of sensor set to 0
int mqSensorValue = 0;

// Counter for watering system
int waterCounter = 0;

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
  pinMode(FAN_PIN, OUTPUT);
  pinMode(MQ_SENSOR_PIN, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  windowServo.attach(5);
}

void loop() {
  
  delay(2000);
  status = WiFi.status();
  
  // While wifi is not yet connected, try to connect
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID 1: ");
      Serial.println(WIFI_AP_1);
      
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP_1, WIFI_PASSWORD_1);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !tb.connected() ) {
    reconnect();
  }
  
    getAndSendTemperatureAndHumidityData();
    getAndSendLightIntensityData();
    getAndSendGroundConductivityData();
    getAirQualityData();
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
  Serial.print("Air  Conductivity: ");  
  Serial.print( mqSensorValue);
  Serial.println("%");
  Serial.println("---------------------------------------------");
  
  // Send data to thingsboard where it can be displayed in a chart
  tb.sendTelemetryFloat("air quality", mqSensorValue);
}

// Function for getting ground conductivity
void getAndSendGroundConductivityData()
{
  //loop through output 100 times at a low delay
  for (int i = 0; i <= 100; i++)
  {
    gConductivity = gConductivity + analogRead(GCONDUCTIVITY_PIN);
    delay(1);
  }

  //devide output by 100 to get the average conductivity
  gConductivity = gConductivity/100.0;
  digitalWrite(WATER_PUMP, LOW);

  // Print data in serial monitor
  Serial.println("Collecting ground conductivity data.");
  Serial.print("Ground Conductivity: ");
  Serial.println(gConductivity);
  Serial.print("Charging Water Pump ⚡: ");
  Serial.print(waterCounter*10);
  Serial.println("%");
  
  if(waterCounter >= 10){
    if(gConductivity >= 140){
      digitalWrite(WATER_PUMP, HIGH);
      Serial.println("|💧 Water Given 💧|");
    }
    waterCounter = 0;
  } else{
    waterCounter++;
  }
  Serial.println("---------------------------------------------");
  //send data to thingsboard where it can be displayed in a chart
  tb.sendTelemetryFloat("ground conductivity", gConductivity);
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
    Serial.print("Attempting to connect to WPA SSID 1: ");
    Serial.println(WIFI_AP_1);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP_1, WIFI_PASSWORD_1);
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
