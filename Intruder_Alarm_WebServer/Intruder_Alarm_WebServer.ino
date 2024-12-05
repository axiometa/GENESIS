// Distance threshold for intruder detection
const int DISTANCE_THRESHOLD = 800;
volatile int distance = 800;  // Initialize with safe distance

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

#define SDA_PIN 47
#define SCL_PIN 48

const char *ssid = "Axiometa";
const char *password = "Axiometa";

WebServer server(80);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Intruder Detection</title>";
  html += "<style>body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }";
  html += ".safe { color: green; font-size: 40px; }";
  html += ".alert { color: red; font-size: 50px; font-weight: bold; }</style>";
  html += "<script>";
  html += "function updateDistance() {";
  html += "fetch('/distance').then(response => response.text()).then(data => {";
  html += "document.getElementById('distance').innerHTML = data;";
  html += "if (parseInt(data) < 800) {";
  html += "document.getElementById('status').innerHTML = 'INTRUDER ALERT!';";
  html += "document.getElementById('status').className = 'alert';";
  html += "} else {";
  html += "document.getElementById('status').innerHTML = 'Safe';";
  html += "document.getElementById('status').className = 'safe';";
  html += "}";
  html += "});";
  html += "}";
  html += "setInterval(updateDistance, 1000);"; // Fetch data every second
  html += "</script></head><body onload=\"updateDistance()\">";
  html += "<h1>Intruder Detection System</h1>";
  html += "<div id=\"status\" class=\"safe\">Safe</div>";
  html += "<p>Distance: <span id=\"distance\">800</span> mm</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleDistance() {
  server.send(200, "text/plain", String(distance));  // Send current distance as plain text
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize VL53L0X sensor
  if (!lox.begin()) {
    Serial.println("Failed to boot VL53L0X sensor.");
    while (1)
      ;
  }

  Serial.println("VL53L0X initialized.");

  // Start continuous ranging
  lox.startRangeContinuous();

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Define web server routes
  server.on("/", handleRoot);         // Serve the main web page
  server.on("/distance", handleDistance);  // Serve the current distance
  server.onNotFound([]() {
    server.send(404, "text/plain", "Page not found");
  });

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // Continuously update distance
  if (lox.isRangeComplete()) {
    int newDistance = lox.readRange();
    if (newDistance > 0) {
      distance = newDistance;  // Update global distance
      //Serial.print("Distance: ");
      //Serial.println(distance);
    } else {
      Serial.println("Error reading range.");
    }
  }

  // Handle web server requests
  server.handleClient();
}
