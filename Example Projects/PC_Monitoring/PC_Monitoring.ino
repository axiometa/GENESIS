#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>


// Wi-Fi credentials
const char* ssid = "Axiometa";      // Replace with your Wi-Fi SSID
const char* password = "Axiometa";  // Replace with your Wi-Fi password

// OpenWeatherMap API details
const char* apiKey = "398120a2cc77359fa3ec688c24c28022";  // Replace with your OpenWeatherMap API key
const char* city = "Vilnius";                             // Replace with your city
const char* apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=Vilnius&appid=398120a2cc77359fa3ec688c24c28022&units=metric";

// NTP Client setup
WiFiUDP ntpUDP;
const int timeZoneOffset = 7200;  // Replace with your offset in seconds (e.g., 7200 for UTC+2)
TwoWire Wire2 = TwoWire(1);       // Use a unique ID (e.g., 1)

NTPClient timeClient(ntpUDP, "pool.ntp.org", timeZoneOffset, 60000);  // Update every 60 seconds

// Initialize displays
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display1(U8G2_R0, U8X8_PIN_NONE, 48, 47);  // Main time display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display2(U8G2_R0, U8X8_PIN_NONE, 48, 47);  // Environmental data
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display3(U8G2_R0, U8X8_PIN_NONE, 12, 3);



// Timekeeping variables
bool colonVisible = true;  // Toggle colon visibility
unsigned long lastColonToggle = 0;
const unsigned long colonInterval = 1000;  // Toggle colon every second

// Environmental data update interval
unsigned long lastEnvUpdate = 0;
const unsigned long envUpdateInterval = 60000;  // Update environmental data every 60 seconds

// Variables to store environmental data
float temperature = 0.0;
float feelsLike = 0.0;
float pressure = 0.0;
float windSpeed = 0.0;
int humidity = 0;
String weather = "Loading...";
unsigned long sunrise = 0;
unsigned long sunset = 0;

void setup() {

  // Initialize displays
  display1.setI2CAddress(0x78);
  display1.begin();
  display1.setFont(u8g2_font_logisoso42_tn);
  display1.clearDisplay();

  display2.setI2CAddress(0x7A);
  display2.begin();
  display2.setFont(u8g2_font_ncenB08_tr);
  display2.clearDisplay();

  Wire2.begin(3, 12);  // Start Wire2 with custom SDA (3) and SCL (12)
  display3.setI2CAddress(0x78);
  display3.begin();
  display3.setFont(u8g2_font_ncenB08_tr);
  display3.clearDisplay();

  // Initialize serial communication
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize NTP client
  timeClient.begin();


  int ledPins[] = { 1, 46, 18, 5, 10, 13 };
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Update and display time
  displayTime();

  // Update environmental data periodically
  if (millis() - lastEnvUpdate >= envUpdateInterval) {
    lastEnvUpdate = millis();
    fetchEnvironmentalData();
  }

  // Display environmental data
  //displayEnvironmentalData();
  displayCpuTemp(49.6);
  flashRandomLEDs();


  delay(10);  // Small delay for smoother performance
}

void flashRandomLEDs() {
  // Array of LED pins
  int ledPins[] = { 1, 46, 18, 5, 10, 13 };
  int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

  // Randomly select an LED to toggle
  int randomIndex = random(0, numLeds);  // Generate a random index
  int selectedPin = ledPins[randomIndex];

  // Toggle the selected LED briefly
  digitalWrite(selectedPin, HIGH);
  delay(1000);  // Brief delay (100ms)
  digitalWrite(selectedPin, LOW);
}


void displayTime() {
  // Update time from NTP
  timeClient.update();
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();

  // Toggle colon visibility
  if (millis() - lastColonToggle >= colonInterval) {
    colonVisible = !colonVisible;
    lastColonToggle = millis();
  }

  // Display hours and minutes
  display1.clearBuffer();
  char timeStr[6];
  sprintf(timeStr, colonVisible ? "%02d:%02d" : "%02d:%02d", hours, minutes);
  display1.setCursor(0, 50);
  display1.print(timeStr);
  display1.sendBuffer();
}

void fetchEnvironmentalData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiUrl);

    int httpCode = http.GET();
    if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      // Parse JSON data
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        // Extract relevant data
        temperature = doc["main"]["temp"];                                     // Temperature in Celsius
        feelsLike = doc["main"]["feels_like"];                                 // Feels like temperature
        pressure = doc["main"]["pressure"];                                    // Atmospheric pressure
        humidity = doc["main"]["humidity"];                                    // Humidity in percentage
        windSpeed = doc["wind"]["speed"];                                      // Wind speed in m/s
        weather = String(doc["weather"][0]["description"].as<const char*>());  // Weather
        sunrise = doc["sys"]["sunrise"];                                       // Sunrise time (UTC)
        sunset = doc["sys"]["sunset"];                                         // Sunset time (UTC)

        Serial.println("Environmental data updated!");
      } else {
        Serial.println("JSON parsing failed!");
      }
    } else {
      Serial.println("HTTP request failed!");
    }

    http.end();
  } else {
    Serial.println("Wi-Fi disconnected!");
  }
}



void displayCpuTemp(float temperature) {
  display2.clearBuffer();

  // Set font for large text
  display2.setFont(u8g2_font_fur30_tf);  // Large font for visibility

  // Convert temperature to a string
  String tempText = String(temperature, 1) + "C";

  // Center the text on the display
  const int screenWidth = 128;  // Display width in pixels
  int textWidth = display2.getStrWidth(tempText.c_str());
  int startX = (screenWidth - textWidth) / 2;  // Calculate horizontal starting position

  // Set cursor and display temperature
  display2.setCursor(startX, 40);  // Adjust Y position as needed
  display2.print(tempText);

  display2.sendBuffer();
}
