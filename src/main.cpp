  #include <ESP8266WiFiMulti.h>
  #include <NewPing.h>
  #include <ESP8266WebServer.h>

  ESP8266WiFiMulti wifiMulti;

  // Ultrasonic sensor ke pins
  #define TRIG_PIN D2
  #define ECHO_PIN D1
  #define MAX_DISTANCE 400

  //Gas sensor ke pins
  #define MQ2_PIN A0
  #define GAS_THRESHOLD 850
  #define BUZZER_PIN D4

  //Ir sensor ke pins + repective led
  #define PIR_SENSOR_PIN D6
  #define LED_ROOM_PIN D5
  
  #define SERVO_PIN D7

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
    Serial.println(WiFi.localIP());
  }

  // Function to check WiFi status and control the LED
  void checkWiFiStatus() {
    digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
    // Serial.println(WiFi.localIP());
  }

  // Function to read the ultrasonic sensor and return the distance
  unsigned int readUltrasonicSensor() {
    return sonar.ping_cm();
  }

  // Function to detect intruders based on sensor reading
  void detectIntruder() {
    unsigned int distance = readUltrasonicSensor();
    
    // Serial.print(distance);
    // Serial.println(" cm");

    // Check for intruders
    if (distance > 0 && distance < 200) { 
      // Serial.println("Intruder Detected!");
    } else {
      // Serial.println("No Intruder.");
    }
  }

  int readSmokeSensor() {
    int sensorValue = analogRead(MQ2_PIN);
    // Serial.print("MQ-2 Sensor Value: ");
    // Serial.println(sensorValue);
    return sensorValue;
  }

  // Function to detect gas
  void detectGas() {
    int gasLevel = readSmokeSensor();
    
    if (gasLevel > GAS_THRESHOLD) {
      digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer
      Serial.println("Gas or Smoke Detected! Warning!");
    } else {
      digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
      Serial.println("Air Quality Normal.");
    }
  }

  void detectMotionWithIR() {
    int motionDetected = digitalRead(PIR_SENSOR_PIN);
    
    if (motionDetected == LOW) {
      digitalWrite(LED_ROOM_PIN, HIGH);
      // Serial.println("Motion Detected: Person Entered Room");
    } else {
      // Serial.println("Maa chuda bkl");
      digitalWrite(LED_ROOM_PIN, LOW);
    }
  }

  ESP8266WebServer server(80);

  void handleRoot() {
    server.send(200, "text/plain", "ESP8266 Home Automation");
  }

  // Endpoint: /status
  void handleStatus() {
    int distance = readUltrasonicSensor();
    int gasLevel = readSmokeSensor();
    int motionDetected = digitalRead(PIR_SENSOR_PIN);

    String json = "{";
    json += "\"distance\":" + String(distance) + ",";
    json += "\"gasLevel\":" + String(gasLevel) + ",";
    json += "\"motionDetected\":" + String(motionDetected);
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");

    server.send(200, "application/json", json);
  }

  void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    connectToWiFi();

    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(LED_ROOM_PIN, OUTPUT);
    digitalWrite(LED_ROOM_PIN, LOW);

    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.begin();
    Serial.println("HTTP server started");

  }

  // Loop Function
  void loop() {
    checkWiFiStatus();
    detectIntruder();
    detectGas();
    detectMotionWithIR();
    delay(30);
    server.handleClient();

  }

  