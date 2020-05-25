#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <Servo.h>
#include "ThingsBoard.h"
#include "SoftwareSerial.h"


// Constants
#define SERVO_PIN 9 // Servo Output Pin
#define SERVO_UPDATE_INTERVAL 20 // Speed of servo position updates
// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define WIFI_AP             "BZiggoA3CF7F4"
#define WIFI_PASSWORD       "rPkcepn38cQm"

#define TOKEN               "nulIlMPhQBOzziPdqUoq"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Servo Variables
int minUs = 0;
int maxUs = 180;
int SetPosition = 0; // ServoMotor Position Setpoint
int Position = 0; // ServoMotor current position
// Control/Timing Variables
long lastServoTime = 0; // keeps track of timestamp since the last servo update occured
Servo ServoMotor;
// Serial driver for ESP
SoftwareSerial soft(2, 3); // RX, TX
// Initialize the Ethernet client object
WiFiEspClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);

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

void setup() {
  Serial.begin(9600);
  soft.begin(9600);
  
  // Initialize Servo
  ServoMotor.attach(SERVO_PIN, minUs, maxUs);
  // Initialize the WiFi and MQTT connections
  
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
}

// RPC Callbacks
const size_t callbacks_size = 2;
RPC_Callback callbacks[] = {
  { "setPos", setPosition },
  { "getPos", getPosition },
};

bool subscribed = false; 

void loop() {
  delay(100);

  if (WiFi.status() == WL_IDLE_STATUS) {
    // Still connecting
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to AP ...");
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    return;
  }

  if (!tb.connected()) {
    subscribed = false;

    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect, retrying ...");
      return;
    }
  }

  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // processTemperatureChange() and processSwitchChange() functions,
    // as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, callbacks_size)) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  Serial.println("Waiting for data...");
  tb.loop();
}

void updateServo()
{
  long currentTime = millis();
  if (currentTime - lastServoTime > SERVO_UPDATE_INTERVAL) {
    lastServoTime = currentTime;
    
    // Approach the Horizontal set point incrementally and update the servo if applicable
    if (Position != SetPosition) {
      Position = SetPosition;
      Serial.print(Position);
      ServoMotor.write(Position);
      }
  }
}
