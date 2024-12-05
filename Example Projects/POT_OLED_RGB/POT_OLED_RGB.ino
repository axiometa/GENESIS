#include <U8g2lib.h>
#include <Wire.h>

// OLED display setup
U8G2_SSD1306_128X64_NONAME_1_HW_I2C OLED(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Potentiometer pin
#define POT_PIN A0

// RGB LED pins
#define RED_PIN 7
#define GREEN_PIN 6
#define BLUE_PIN 5

void setup() {
  // Initialize the OLED display
  OLED.begin();
  OLED.setBusClock(400000);

  // Initialize RGB LED pins as output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial)
    ;  // Wait for serial monitor to open
  Serial.println("Bar Graph with Potentiometer and RGB LED Example.");
}

void loop() {
  // Read the potentiometer value (0-1023)
  int potValue = analogRead(POT_PIN);

  // Map the potentiometer value to the width of the bar (0 to 128 pixels)
  int barWidth = map(potValue, 0, 1023, 0, 128);

  // Map the potentiometer value to RGB LED brightness (0 to 255)
  int redValue = map(potValue, 0, 1023, 0, 255);
  int greenValue = map(potValue, 0, 1023, 0, 255); // Inverse mapping for green
  int blueValue = map(potValue, 0, 1023, 0, 255);

  // Set RGB LED colors based on potentiometer value
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);

  // Draw the bar graph
  drawBarGraph(barWidth);

  // Output potentiometer value and LED colors to Serial Monitor
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);
  Serial.print(" | Bar Width: ");
  Serial.print(barWidth);
  Serial.print(" | RGB LED (R,G,B): ");
  Serial.print(redValue);
  Serial.print(",");
  Serial.print(greenValue);
  Serial.print(",");
  Serial.println(blueValue);
}

// Function to draw a bar graph
void drawBarGraph(int barWidth) {
  OLED.firstPage();
  do {
    // Draw a rectangle for the bar graph
    int barHeight = 20;   // Height of the bar
    int barY = 32 - (barHeight / 2);  // Center the bar vertically

    // Draw the filled bar
    OLED.drawBox(0, barY, barWidth, barHeight);

    // Draw the border of the bar (optional)
    OLED.drawFrame(0, barY, 128, barHeight);

  } while (OLED.nextPage());
}
