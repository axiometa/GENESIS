#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <Wire.h>

// WiFi credentials
const char* ssid = "Axiometa";
const char* password = "Axiometa";

// Motor pin definitions
#define A1B 4   // RIGHT BACK
#define A1A 17  // RIGHT FRONT
#define A2B 1   // LEFT BACK
#define A2A 46  // LEFT FRONT

// Motor control states (active low)
#define MOTOR_STOP HIGH
#define MOTOR_FORWARD LOW
#define MOTOR_BACKWARD LOW

// Create web server
WebServer server(80);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 48, 47);  // Main time display


// HTML page with joystick control
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Robot Control</title>
    <style>
        .button {
            width: 100px;
            height: 100px;
            font-size: 20px;
            margin: 10px;
            text-align: center;
        }
        #container {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            align-items: center;
            height: 300px;
        }
        #joystick {
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        .row {
            display: flex;
            justify-content: center;
        }
    </style>
</head>
<body>
    <h1>Robot Control</h1>
    <div id="joystick">
        <div class="row">
            <button class="button" onclick="sendCommand('forward')">▲</button>
        </div>
        <div class="row">
            <button class="button" onclick="sendCommand('left')">◄</button>
            <button class="button" onclick="sendCommand('stop')">■</button>
            <button class="button" onclick="sendCommand('right')">►</button>
        </div>
        <div class="row">
            <button class="button" onclick="sendCommand('backward')">▼</button>
        </div>
    </div>
    <script>
        function sendCommand(command) {
            fetch(`/control?cmd=${command}`);
        }
    </script>
</body>
</html>
)rawliteral";

// Motor control function
void controlMotors(String command) {
  if (command == "forward") {
    digitalWrite(A1A, MOTOR_FORWARD);
    delay(50);
    digitalWrite(A1B, MOTOR_STOP);
    delay(50);
    digitalWrite(A2A, MOTOR_FORWARD);
    delay(50);
    digitalWrite(A2B, MOTOR_STOP);
    delay(50);
  } else if (command == "backward") {
    digitalWrite(A1A, MOTOR_STOP);
    delay(50);
    digitalWrite(A1B, MOTOR_BACKWARD);
    delay(50);
    digitalWrite(A2A, MOTOR_STOP);
    delay(50);
    digitalWrite(A2B, MOTOR_BACKWARD);
    delay(50);
  } else if (command == "left") {
    digitalWrite(A1A, MOTOR_STOP);
    delay(50);
    digitalWrite(A1B, MOTOR_BACKWARD);
    delay(50);
    digitalWrite(A2A, MOTOR_FORWARD);
    delay(50);
    digitalWrite(A2B, MOTOR_STOP);
    delay(50);
  } else if (command == "right") {
    digitalWrite(A1A, MOTOR_FORWARD);
    delay(50);
    digitalWrite(A1B, MOTOR_STOP);
    delay(50);
    digitalWrite(A2A, MOTOR_STOP);
    delay(50);
    digitalWrite(A2B, MOTOR_BACKWARD);
    delay(50);
  } else if (command == "stop") {
    digitalWrite(A1A, MOTOR_STOP);
    delay(50);
    digitalWrite(A1B, MOTOR_STOP);
    delay(50);
    digitalWrite(A2A, MOTOR_STOP);
    delay(50);
    digitalWrite(A2B, MOTOR_STOP);
    delay(50);
  }
}

// Web server handler for the root page
void handleRoot() {
  server.send(200, "text/html", webpage);
}

// Web server handler for motor commands
void handleControl() {
  if (server.hasArg("cmd")) {
    String command = server.arg("cmd");
    controlMotors(command);
    server.send(200, "text/plain", "Command received: " + command);
  } else {
    server.send(400, "text/plain", "Bad Request: No command provided");
  }
}

void setup() {
  // Initialize serial
  Serial.begin(115200);
  u8g2.begin();
  displayEyes();

  // Initialize motor pins
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(A2A, OUTPUT);
  pinMode(A2B, OUTPUT);

  // Stop motors initially
  controlMotors("stop");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure web server routes
  server.on("/", handleRoot);
  server.on("/control", handleControl);

  // Start web server
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}


void displayEyes() {
  u8g2.clearBuffer();  // Clear the internal memory

  // Draw the outer eye (ellipse shape)
  u8g2.drawEllipse(64, 32, 50, 20, U8G2_DRAW_ALL);

  // Draw the upper eye crease (simulating the upper half of an ellipse)
  for (int x = -48; x <= 48; x++) {
    int y = sqrt(10 * 10 * (1 - (x * x) / (48.0 * 48.0)));  // Adjust height and width
    u8g2.drawPixel(64 + x, 22 - y);
  }

  // Draw the lower eye crease (simulating the lower half of an ellipse)
  for (int x = -48; x <= 48; x++) {
    int y = sqrt(10 * 10 * (1 - (x * x) / (48.0 * 48.0)));
    u8g2.drawPixel(64 + x, 42 + y);
  }

  // Draw the iris
  u8g2.drawDisc(64, 32, 10);  // Outer iris
  u8g2.setDrawColor(0);
  u8g2.drawCircle(64, 32, 9);  // Inner ring
  u8g2.setDrawColor(1);
  u8g2.drawDisc(64, 32, 5);  // Central iris

  // Draw the pupil
  u8g2.setDrawColor(0);      // Set to black
  u8g2.drawDisc(64, 32, 4);  // Pupil
  u8g2.setDrawColor(1);      // Back to white

  // Add a highlight to the pupil
  u8g2.setDrawColor(0);
  u8g2.drawDisc(66, 30, 1);  // Small highlight
  u8g2.setDrawColor(1);

  // Draw eyelashes
  u8g2.drawLine(14, 12, 30, 20);   // Top left eyelash
  u8g2.drawLine(98, 20, 114, 12);  // Top right eyelash
  u8g2.drawLine(64, 12, 64, 24);   // Center eyelash

  u8g2.sendBuffer();  // Transfer internal memory to the display
  delay(10);          // Add a delay
}
