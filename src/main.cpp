  #include <ESP8266WiFiMulti.h>
  #include <NewPing.h>
  #include <ESP8266WebServer.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <Servo.h>

  
  Servo myServo;
  ESP8266WiFiMulti wifiMulti;
  ESP8266WebServer server(80);


  // Ultrasonic sensor ke pins
  #define TRIG_PIN D2
  #define ECHO_PIN D1
  #define MAX_DISTANCE 400
  NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);


  //Gas sensor ke pins
  #define MQ2_PIN A0
  #define GAS_THRESHOLD 850
  #define BUZZER_PIN D4

  //Ir sensor ke pins + repective led
  #define PIR_SENSOR_PIN D6
  #define LED_ROOM_PIN D5
  

  //Display 
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64

  // Use GPIO9 and GPIO10 for SDA and SCL
  #define OLED_SDA 9
  #define OLED_SCL 10

  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  #define SERVO_PIN D7

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


  String correctPassword = "1234";

void handlePassword() {
  if (server.hasArg("password")) {
    String receivedPassword = server.arg("password");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print("Pwd: ");
    display.println(receivedPassword);
    display.display();

    if (receivedPassword == correctPassword) {
      display.setCursor(0, 30);
      display.print("Access OK");
      display.display();
      Serial.println("Correct Password!");
      
    } else {
      display.setCursor(0, 30);
      display.print("Access Denied");
      display.display();
      Serial.println("Wrong Password!");
    }
    server.send(200, "text/plain", "Password received");
  } else {
    server.send(400, "text/plain", "Missing password");
  }
}


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
  
    // Setup built-in LED for WiFi status
    pinMode(LED_BUILTIN, OUTPUT);
  
    // Connect to WiFi
    connectToWiFi();
  
    // Setup PIR sensor and room light LED
    pinMode(PIR_SENSOR_PIN, INPUT);
    pinMode(LED_ROOM_PIN, OUTPUT);
    digitalWrite(LED_ROOM_PIN, LOW);
    
    myServo.attach(SERVO_PIN);
    myServo.write(0); // Ensure it's closed initially

    // Initialize OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
  
    // Set up HTTP server routes
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.on("/password", HTTP_POST, handlePassword);
  
    // Start the web server
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