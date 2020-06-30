#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <ThingsBoard.h>
#include <NeoPatterns.h>

// Include selfmade libraries
#include "air_quality.h"
#include "ground_conductivity.h"
#include "light_intensity.h"
#include "temperature_and_humidity.h"

// Wifi initialization
#define WIFI_AP "Bakker Netwerk"
#define WIFI_PASSWORD "12frituurbrood21"

// Get thingsboard token
#define TOKEN "AX2c6NznHp82T68MIi0S"

//Ground Conductivity sensor
int GRESISTANCE_PIN = A1;

//Water pump
int WATER_PUMP = 7;

// Set IP for server
char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Initialize ESP client
ThingsBoard tb(espClient);

// RX, TX
SoftwareSerial soft(2, 3); 

// Wifi variables
int status = WL_IDLE_STATUS;
unsigned long lastSend;

// Digital pin 8 will be used as indicator for the MQ-135 sensor
int MQ_SENSOR_PIN = 8;

// Analog pin 2 will be used for MQ-135 sensor
int mqSensor = A2;

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

//Led strip
#define LED_STRIP_PIN 10

// Neopixel
#define PIN_NEOPIXEL_JEWEL 6

// Fan 
#define FAN_PIN 13

// Temperature and humidity sensor values
#define DHTPIN 4
#define DHTTYPE DHT11

// Initializing DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// The NeoPatterns instances
NeoPatterns jewel = NeoPatterns(7, PIN_NEOPIXEL_JEWEL, NEO_RGBW + NEO_KHZ800);

// Declare library objects
MQ135 airSensor(MQ_SENSOR_PIN);

FC_28 groundResistance(WATER_PUMP);

Photocell lightIntensity(PHOTOCELL_PIN);

DHT11Object temperatureAndHumidity(FAN_PIN);

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  dht.begin(); // This sets the pin
  InitWiFi();
  jewel.begin(); // This sets the pin
}

void loop() {
  // Set loop delay
  delay(2000);
  status = WiFi.status();
  
  // While wifi is not yet connected, try to connect
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(1000);
    }
    Serial.println("Connected to AP");
  }

  if ( !tb.connected() ) {
    reconnect();
  }

  // Call library functions 
  airSensor.getAirQualityData(mqSensor, tb);
  // Set colors of Neopixel after every library call
  NeoAnimation(0, 255, 0);
  groundResistance.getGroundResistanceData(GRESISTANCE_PIN, WATER_PUMP, tb);
  NeoAnimation(125, 255, 0);
  lightIntensity.getAndSendLightIntensityData(PHOTOCELL_PIN, LED_STRIP_PIN, tb);
  NeoAnimation(255, 125, 0);
  temperatureAndHumidity.getAndSendTemperatureAndHumidityData(FAN_PIN, tb, dht);
  NeoAnimation(255, 0, 0);
    
  tb.loop();
}

// Neopixel animation function
void NeoAnimation(int green, int red, int blue){
  jewel.ColorWipe(jewel.Color(green, red, blue), 50, 18);
  jewel.updateAndWaitForPatternToStop();
}

// WiFi initialization function
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

// Reconnect to WiFi function
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
