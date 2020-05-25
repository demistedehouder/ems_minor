#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <ThingsBoard.h>
#include <Servo.h>

#define WIFI_AP "BZiggoA3CF7F4"
#define WIFI_PASSWORD "rPkcepn38cQm"

#define TOKEN "nulIlMPhQBOzziPdqUoq"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

// Speed of servo position updates
#define SERVO_UPDATE_INTERVAL 20 
// Servo Output Pin
#define SERVO_PIN 9 

//photocell state
int current = 0;
int last = -1;

char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

ThingsBoard tb(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

// Initialize servo
Servo windowServo;

// Servo variables
int minUs = 500;
int maxUs = 2400;
int SetPosition = 0; // windowServo Position Setpoint
int Position = 0; // windowServo current position
// Control/Timing Variables
long lastServoTime = 0; // keeps track of timestamp since the last servo update occured

// Processes function for RPC call "getPos"
RPC_Response getPosition(const RPC_Data &data)
{
  Serial.println("Received the get Position Method");
  return RPC_Response(NULL, SetPosition);
}

// Processes function for RPC call "setPos"
RPC_Response setPosition(const RPC_Data &data)
{
  Serial.print("Received the Set Position method: ");
  SetPosition = data;
  Serial.println(SetPosition);
  
  return RPC_Response(NULL, SetPosition);
}

// RPC Callbacks
RPC_Callback callbacks[] = {
  { "setPos", setPosition },
  { "getPos", getPosition },
};

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  dht.begin();
  InitWiFi();
  lastSend = 0;
  // Initialize Servo
  windowServo.attach(SERVO_PIN, minUs, maxUs);
}

void loop() {
  status = WiFi.status();
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

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    getAndSendLightIntensityData();
    lastSend = millis();
  }
#define SERVO_UPDATE_INTERVAL 20 // Speed of servo position updates
  tb.loop();
}

void updateServo()
{
  // Approach the Horizontal set point incrementally and update the servo if applicable
  if (Position != SetPosition)
  {
    Position = SetPosition;
    windowServo.write(Position);
  }
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

  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);
}

void getAndSendLightIntensityData()
{
  Serial.println("Collecting light intensity data.");
  
  //grab the current state of the photocell
  current = analogRead(PHOTOCELL_PIN);

  //return if the value hasn't changed
  if(current == last)
    return;
    
  Serial.println("Light intensity");  
  Serial.println(current);
  tb.sendTelemetryFloat("light intensity", current);
  last = current;
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
