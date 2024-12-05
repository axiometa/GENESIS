#include <U8g2lib.h>

// Initialize the OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

#define ANALOG_PIN A1   // Pin for analog input
#define BUZZER_PIN A10  // Pin for the buzzer
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BASE_FREQUENCY 10  // Minimum frequency in Hz
#define MAX_FREQUENCY 90  // Maximum frequency in Hz

void setup() {
  // Start the display
  u8g2.begin();
  // Start serial communication for debugging (optional)
  Serial.begin(9600);
}

void loop() {
  // Read the analog value and map it to a frequency range
  int analogValue = analogRead(ANALOG_PIN);
  int frequency = map(analogValue, 0, 1023, BASE_FREQUENCY, MAX_FREQUENCY);

  // Play the frequency through the buzzer
  tone(BUZZER_PIN, frequency);

  // Clear the display buffer
  u8g2.clearBuffer();

  // Draw the sine wave
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    // Map the frequency to the wave's period
    float waveLength = (float)SCREEN_WIDTH / (frequency / 10.0); // Adjust scale for better visualization
    float y = (SCREEN_HEIGHT / 4) * sin(2 * PI * x / waveLength); // Calculate sine value
    int yPosition = SCREEN_HEIGHT / 2 - y;                       // Center the wave vertically
    u8g2.drawPixel(x, yPosition);                                // Draw the pixel
  }

  // Send the buffer to the display
  u8g2.sendBuffer();

  // Add a small delay for smooth updates
  delay(30);
}