#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <U8g2lib.h>

// ---------------------------
// Pin Definitions
// ---------------------------

// Potentiometers
const int lowThresholdPin = 2;
const int highThresholdPin = 3;

// RGB LED1 Pins (Low Threshold Indicator)
const int LED1_RED = 38;
const int LED1_GREEN = 14;
const int LED1_BLUE = 8;

// RGB LED2 Pins (High Threshold Indicator)
const int LED2_RED = 39;
const int LED2_GREEN = 40;
const int LED2_BLUE = 9;

// ---------------------------
// Object Initializations
// ---------------------------

// Initialize VL53L0X sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Initialize U8G2 OLED (Example for SSD1306 128x64, adjust if different)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// ---------------------------
// Push-Up Counter Variables
// ---------------------------

int pushUpCount = 0;
bool inUpPosition = false;

// Threshold Values
int lowThreshold = 300;   // Initial value, will be overridden by pot
int highThreshold = 500;  // Initial value, will be overridden by pot

// Debounce Variables
unsigned long lastChangeTime = 0;
const unsigned long debounceDelay = 200;  // 200ms

// ---------------------------
// Setup Function
// ---------------------------

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);

  // Wait until serial port opens for native USB devices
  while (!Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X Push-Up Counter");

  // Initialize I2C for VL53L0X and OLED on GPIO47 (SDA) and GPIO48 (SCL)
  Wire.begin(47, 48);

  // Initialize VL53L0X sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1)
      ;
  }
  Serial.println(F("VL53L0X API Continuous Ranging example"));

  // Start continuous ranging
  lox.startRangeContinuous();

  // Initialize OLED
  u8g2.begin();

  // Initialize RGB LEDs as OUTPUT
  pinMode(LED1_RED, OUTPUT);
  pinMode(LED1_GREEN, OUTPUT);
  pinMode(LED1_BLUE, OUTPUT);

  pinMode(LED2_RED, OUTPUT);
  pinMode(LED2_GREEN, OUTPUT);
  pinMode(LED2_BLUE, OUTPUT);

  // Initialize LEDs to OFF
  setLEDColor(LED1_RED, LED1_GREEN, LED1_BLUE, 0, 0, 0);
  setLEDColor(LED2_RED, LED2_GREEN, LED2_BLUE, 0, 0, 0);
}

// ---------------------------
// Loop Function
// ---------------------------

void loop() {
  // Read Potentiometer Values and Map to Thresholds
  lowThreshold = map(analogRead(lowThresholdPin), 0, 4095, 50, 1000);  // Adjust mapping as needed
  highThreshold = map(analogRead(highThresholdPin), 0, 4095, 100, 1000);

  // Debugging: Print Thresholds
  Serial.print("Low Threshold: ");
  Serial.println(lowThreshold);
  Serial.print("High Threshold: ");
  Serial.println(highThreshold);

  // Read distance from VL53L0X sensor using Adafruit example method
  if (lox.isRangeComplete()) {
    uint16_t distance = lox.readRange();

    if (lox.timeoutOccurred()) {
      Serial.println(F("VL53L0X Timeout!"));
      // Optionally handle timeout
    } else {
      Serial.print(F("Distance (mm): "));
      Serial.println(distance);

      // Push-up logic
      if (distance < lowThreshold) {
        if (inUpPosition) {
          unsigned long currentTime = millis();
          if (currentTime - lastChangeTime > debounceDelay) {
            pushUpCount++;
            inUpPosition = false;
            lastChangeTime = currentTime;
            Serial.print(F("Push-up Count: "));
            Serial.println(pushUpCount);
          }
        }
      } else if (distance > highThreshold) {
        inUpPosition = true;
      }

      // Update LEDs based on distance
      updateLEDs(distance);

      // Update OLED display
      updateDisplay(pushUpCount, lowThreshold, highThreshold, distance);
    }
  }

  delay(50);  // Short delay to prevent excessive processing
}

// ---------------------------
// Helper Functions
// ---------------------------

/**
 * Sets the color of an RGB LED.
 * @param redPin   GPIO pin connected to the red LED.
 * @param greenPin GPIO pin connected to the green LED.
 * @param bluePin  GPIO pin connected to the blue LED.
 * @param red      Intensity of the red color (0-255).
 * @param green    Intensity of the green color (0-255).
 * @param blue     Intensity of the blue color (0-255).
 */
void setLEDColor(int redPin, int greenPin, int bluePin, int red, int green, int blue) {
  // Assuming Common Cathode LEDs
  // For Common Anode, invert the values (255 - value)
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

/**
 * Updates both RGB LEDs based on the current distance.
 * LED1: Low Threshold Indicator
 * LED2: High Threshold Indicator
 * @param distance Current distance measurement in millimeters.
 */
void updateLEDs(int distance) {
  // ---------------------------
  // Update LED1 (Low Threshold Indicator)
  // ---------------------------
  if (distance < lowThreshold) {
    // Green - Low position achieved
    setLEDColor(LED1_RED, LED1_GREEN, LED1_BLUE, 0, 255, 0);
  } else if (distance < (lowThreshold + (highThreshold - lowThreshold) / 2)) {
    // Yellow - Approaching Low position
    setLEDColor(LED1_RED, LED1_GREEN, LED1_BLUE, 255, 255, 0);
  } else {
    // Red - Not in Low position
    setLEDColor(LED1_RED, LED1_GREEN, LED1_BLUE, 255, 0, 0);
  }

  // ---------------------------
  // Update LED2 (High Threshold Indicator)
  // ---------------------------
  if (distance > highThreshold) {
    // Green - High position achieved
    setLEDColor(LED2_RED, LED2_GREEN, LED2_BLUE, 0, 255, 0);
  } else if (distance > (highThreshold - (highThreshold - lowThreshold) / 2)) {
    // Yellow - Approaching High position
    setLEDColor(LED2_RED, LED2_GREEN, LED2_BLUE, 255, 255, 0);
  } else {
    // Red - Not in High position
    setLEDColor(LED2_RED, LED2_GREEN, LED2_BLUE, 255, 0, 0);
  }
}

/**
 * Updates the OLED display with the current push-up count and thresholds.
 * @param count        Current number of push-ups performed.
 * @param lowThreshold Current low threshold value.
 * @param highThreshold Current high threshold value.
 */
void updateDisplay(int count, int lowThreshold, int highThreshold, int distance) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);  // Choose a suitable font
  u8g2.drawStr(0, 10, "Push-Up Counter");

  u8g2.setFont(u8g2_font_ncenB14_tr);
  String countStr = "Count: " + String(count);
  u8g2.drawStr(0, 30, countStr.c_str());

  // Display Low and High Threshold Values
  u8g2.setFont(u8g2_font_ncenB08_tr);
  String lowStr = "Low: " + String(lowThreshold);
  u8g2.drawStr(0, 50, lowStr.c_str());

  String highStr = "High: " + String(highThreshold);
  u8g2.drawStr(60, 50, highStr.c_str());

  String distanceStr = "State: " + String(distance);
  u8g2.drawStr(0, 60, distanceStr.c_str());

  u8g2.sendBuffer();
}
