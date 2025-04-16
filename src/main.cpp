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
  #define GAS_THRESHOLD 960
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

  //ldr ke pins + led
  // #define LDR_PIN A0
  // #define FENCE_LED_PIN D0
  // #define LDR_THRESHOLD 300  // Adjust based on ambient lighting

  //raindrop ke pins
  #define RAIN_SENSOR_PIN D3
  #define PUMP_MOTOR_PIN D8
  #define RAIN_THRESHOLD 715  // Adjust this based on your sensor readings



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


  int readSmokeSensor() {
    int sensorValue = analogRead(MQ2_PIN);
    // Serial.print("MQ-2 Sensor Value: ");
    // Serial.println(sensorValue);
    return sensorValue;
  }

  // Function to detect gas
  void detectGas() {
    int gasLevel = readSmokeSensor();
    
    if (gasLevel < GAS_THRESHOLD) {
      digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer
      Serial.println("Air Quality Normal.");
    } else {
      digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
      Serial.println("Gas or Smoke Detected! Warning!");
    }
  }

  void controlPumpWithRainSensor() {
    int rainValue = digitalRead(RAIN_SENSOR_PIN);
    Serial.print("Rain Sensor Value: ");
    Serial.println(rainValue);
  
    if (rainValue == 1) {
      digitalWrite(PUMP_MOTOR_PIN, HIGH);  // Turn off pump
      Serial.println("Less Water. Pump ON.");
    } else {
      digitalWrite(PUMP_MOTOR_PIN, LOW); // Turn on pump
      Serial.println("Water detected. Pump OFF.");
    }
  }
  

  void detectMotionWithIR() {
    int motionDetected = digitalRead(PIR_SENSOR_PIN);
    
    if (motionDetected == LOW) {
      digitalWrite(LED_ROOM_PIN, HIGH);
      delay(2000);
      // Serial.println("Motion Detected: Person Entered Room");
    } else {
      // Serial.println("Maa chuda bkl");
      digitalWrite(LED_ROOM_PIN, LOW);
    }
  }

  // void controlFenceLight() {
  //   int ldrValue = analogRead(LDR_PIN);
  //   Serial.print("LDR Value: ");
  //   Serial.println(ldrValue);
  
  //   if (ldrValue > LDR_THRESHOLD) {
  //     digitalWrite(FENCE_LED_PIN, HIGH); // Turn on fence light
  //   } else {
  //     digitalWrite(FENCE_LED_PIN, LOW);  // Turn off
  //   }
  // }
  


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
      myServo.write(90);
      delay(1000);
      myServo.write(00);
    
      
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
    int gasLevel = readSmokeSensor();
    int motionDetected = digitalRead(PIR_SENSOR_PIN);
    // int ldrValue = analogRead(LDR_PIN);
    int rainValue = digitalRead(RAIN_SENSOR_PIN);
  
    String json = "{";
    // json += "\"distance\":" + String(distance) + ",";
    json += "\"gasLevel\":" + String(gasLevel) + ",";
    json += "\"motionDetected\":" + String(motionDetected) + ",";
    // json += "\"ldrValue\":" + String(ldrValue) + ",";
    json += "\"rainValue\":" + String(rainValue);
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

    // pinMode(FENCE_LED_PIN, OUTPUT);
    // digitalWrite(FENCE_LED_PIN, LOW);
    pinMode(PUMP_MOTOR_PIN, OUTPUT);
    digitalWrite(PUMP_MOTOR_PIN, LOW);  // Initially off


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
    detectGas();
    detectMotionWithIR();
    controlPumpWithRainSensor();
    // controlFenceLight();
    server.handleClient();
    delay(300);

  }


