#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <DHT11.h>

// Initialize displays

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C(const u8g2_cb_t *rotation, uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE)

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display1(U8G2_R0, U8X8_PIN_NONE); // Hours & Minutes
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display2(U8G2_R0, U8X8_PIN_NONE); // Date, Temperature, etc.

// Initialize DHT11
DHT11 dht11(A8); // Connect the DHT11 to pin A8

// Timekeeping variables
unsigned long previousMillis = 0;
unsigned long colonMillis = 0;
const unsigned long interval = 60000; // 1 minute
const unsigned long colonInterval = 1000; // 1 second
bool colonVisible = true; // Tracks the visibility of the `:`
int hours =20;   // Default start time
int minutes = 42;

// Date variables
String date = "23 Nov 2024";
String weekday = "Thursday";
int year = 2024;

// Temperature and humidity variables
int temperature = 0;
int humidity = 0;

void setup() {
  // Initialize displays
  display1.setI2CAddress(0x78);
  display1.begin();
  display1.setFont(u8g2_font_logisoso42_tn); // Large font for time display
  display1.clearDisplay();

  display2.setI2CAddress(0x7A);
  display2.begin();
  display2.setFont(u8g2_font_ncenB08_tr); // Regular font for text
  display2.clearDisplay();

  // Initialize serial communication for debugging DHT11
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  // Increment time every minute
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours >= 24) {
        hours = 0; // Reset hours after 24
      }
    }
  }

  // Toggle colon visibility every second
  if (currentMillis - colonMillis >= colonInterval) {
    colonMillis = currentMillis;
    colonVisible = !colonVisible; // Toggle colon visibility
  }

  // Read temperature and humidity from DHT11
  int result = dht11.readTemperatureHumidity(temperature, humidity);
  if (result != 0) {
    // Handle sensor error if needed
    Serial.println(DHT11::getErrorString(result));
  }

  // Display hours and minutes on Screen 1
  display1.clearBuffer();
  char timeStr[6];
  if (colonVisible) {
    sprintf(timeStr, "%02d:%02d", hours, minutes); // Display with colon
  } else {
    sprintf(timeStr, "%02d:%02d", hours, minutes); // Display without colon
  }
  display1.setCursor(0, 50); // Adjust position for full-screen display
  display1.print(timeStr);
  display1.sendBuffer();

  // Display date, day, year, and temperature/humidity on Screen 2
  display2.clearBuffer();
  display2.setCursor(0, 10);
  display2.print(date);

  display2.setCursor(0, 25);
  display2.print(weekday);

  display2.setCursor(0, 40);
  display2.print("Temp: ");
  display2.print(temperature);
  display2.print(" C");

  display2.setCursor(0, 55);
  display2.print("Hum: ");
  display2.print(humidity);
  display2.print(" %");

  display2.sendBuffer();

  delay(10); // Small delay for smoother performance
}
