#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* ssid = "Axiometa";      // Replace with your Wi-Fi SSID
const char* password = "Axiometa";  // Replace with your Wi-Fi password

// OpenWeatherMap API details
const char* apiKey = "398120a2cc77359fa3ec688c24c28022";  // Replace with your OpenWeatherMap API key
const char* city = "Vilnius";                             // Replace with your city
const char* apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=Vilnius&appid=398120a2cc77359fa3ec688c24c28022&units=metric";

// NTP Client setup
WiFiUDP ntpUDP;
const int timeZoneOffset = 7200;                                      // Replace with your offset in seconds (e.g., 7200 for UTC+2)
NTPClient timeClient(ntpUDP, "pool.ntp.org", timeZoneOffset, 60000);  // Update every 60 seconds

// Initialize displays
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display1(U8G2_R0, U8X8_PIN_NONE, 48, 47);  // Main time display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display2(U8G2_R0, U8X8_PIN_NONE, 48, 47);  // Environmental data

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
  displayEnvironmentalData();

  delay(10);  // Small delay for smoother performance
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

void displayEnvironmentalData() {
  display2.clearBuffer();

  // Weather description at the top center
  const int screenWidth = 128;
  display2.setFont(u8g2_font_ncenB08_tr);                     // Small font for description
  String weatherText = "Currently: " + weather;               // Combine text for centering
  int textWidth = display2.getStrWidth(weatherText.c_str());  // Calculate text width
  int startX = (screenWidth - textWidth) / 2;                 // Calculate horizontal starting position
  display2.setCursor(startX, 10);
  display2.print(weatherText);

  // Temperature and humidity in top-left corner
  display2.setFont(u8g2_font_profont15_mf);  // Compact font for concise info
  display2.setCursor(0, 25);
  display2.print(temperature, 1);
  display2.print("C");  // Show temperature
  display2.setCursor(40, 25);
  display2.print(humidity);
  display2.print("%");  // Show humidity

  // Wind speed with icon in the center
  display2.setFont(u8g2_font_open_iconic_weather_2x_t);  // Weather icons font
  display2.setCursor(0, 50);
  display2.print("B");                       // Wind icon
  display2.setFont(u8g2_font_profont15_mf);  // Switch back to regular font
  display2.setCursor(25, 55);
  display2.print(windSpeed, 1);
  display2.print("m/s");

  // Atmospheric pressure in the bottom-right corner
  display2.setCursor(70, 55);
  display2.print(pressure, 1);
  display2.print("hPa");

  display2.sendBuffer();
}
