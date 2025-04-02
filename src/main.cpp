// #include <ESP8266WiFiMulti.h>

// ESP8266WiFiMulti wifiMulti;

// void setup() {
//   Serial.begin(115200);
//   pinMode(LED_BUILTIN, OUTPUT);

//   wifiMulti.addAP("Noway", "22blc1128");

//   while (wifiMulti.run() != WL_CONNECTED) {
//     delay(100);
//   }

//   Serial.println("Connected to WiFi");
// }

// void loop() {
//   digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED ? LOW : HIGH); // ESP8266 LED is active LOW
// }

#include <ESP8266WiFiMulti.h>
#include <NewPing.h>

ESP8266WiFiMulti wifiMulti;

// Ultrasonic sensor pins
#define TRIG_PIN D2
#define ECHO_PIN D1
#define MAX_DISTANCE 400

//Gas sensor ke pins
#define MQ2_PIN A0
#define GAS_THRESHOLD 850

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  wifiMulti.addAP("Noway", "22blc1128");

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

// Function to check WiFi status and control the LED
void checkWiFiStatus() {
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
}

// Function to read the ultrasonic sensor and return the distance
unsigned int readUltrasonicSensor() {
  return sonar.ping_cm();
}

// Function to detect intruders based on sensor reading
void detectIntruder() {
  unsigned int distance = readUltrasonicSensor();
  
  Serial.print(distance);
  Serial.println(" cm");

  // Check for intruders
  if (distance > 0 && distance < 200) { 
    Serial.println("Intruder Detected!");
  } else {
    Serial.println("No Intruder.");
  }
}

int readSmokeSensor() {
  int sensorValue = analogRead(MQ2_PIN);
  Serial.print("MQ-2 Sensor Value: ");
  Serial.println(sensorValue);
  return sensorValue;
}

// Function to detect gas
void detectGas() {
  int gasLevel = readSmokeSensor();
  
  if (gasLevel > GAS_THRESHOLD) {
    Serial.println("Gas or Smoke Detected! Warning!");
  } else {
    Serial.println("Air Quality Normal.");
  }
}

// Setup Function
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  connectToWiFi();
}

// Loop Function
void loop() {
  checkWiFiStatus();
  detectIntruder();
  detectGas();
  delay(500);
}

