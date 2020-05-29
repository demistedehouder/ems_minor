#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <ThingsBoard.h>

#define WIFI_AP "BZiggoA3CF7F4"
#define WIFI_PASSWORD "rPkcepn38cQm"

#define TOKEN "nulIlMPhQBOzziPdqUoq"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11

//analog pin 0 for light sensor
#define PHOTOCELL_PIN A0

//Ground humidity sensor
#define GHUMIDITY_PIN A1

//photocell state
int current = 0;
int last = -1;

//ground humidity value
float gHumidity = 0;

char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

ThingsBoard tb(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  dht.begin();
  InitWiFi();
  lastSend = 0;
}

void loop() {
  delay(2000);
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
    getAndSendGroundHumidityData();
    lastSend = millis();
  }
  tb.loop();
}

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
  current = analogRead(PHOTOCELL_PIN);

  //return if the value hasn't changed
  if(current == last)
    return;
    
  Serial.print("Light intensity: ");  
  Serial.println(current);
  Serial.println("---------------------------------------------");
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
