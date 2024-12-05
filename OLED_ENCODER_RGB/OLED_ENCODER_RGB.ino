#include <U8g2lib.h>
#include <Wire.h>
#include <RotaryEncoder.h>

// OLED display setup
U8G2_SSD1306_128X64_NONAME_1_HW_I2C OLED(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Encoder setup
#define PIN_IN1 3
#define PIN_IN2 4
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

// Button pin (D9)
#define BUTTON_PIN 9

// RGB LED pins
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7

// Menu and Brightness variables
int menuIndex = 0;  // Tracks which menu item is highlighted
int mode = 0;       // 0 = Menu, 1 = Adjust brightness
int brightness[3] = {128, 128, 128};  // Store brightness for RED, GREEN, BLUE

void setup() {
  // Initialize the OLED display
  OLED.begin();
  OLED.setBusClock(400000);

  // Initialize Serial for debugging
  Serial.begin(9600);

  // Initialize button and RGB pins
  pinMode(BUTTON_PIN, INPUT);  // D9 as input with pullup
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Set initial brightness
  analogWrite(RED_PIN, brightness[0]);
  analogWrite(GREEN_PIN, brightness[1]);
  analogWrite(BLUE_PIN, brightness[2]);
}

void loop() {
  encoder.tick();  // Update encoder state
  static int lastPos = -1;

  // Read encoder position for menu scrolling or brightness adjustment
  int newPos = encoder.getPosition();

  // If we are in menu mode
  if (mode == 0) {
    if (newPos != lastPos) {
      if (newPos > lastPos) {
        menuIndex = (menuIndex + 1) % 3;  // Scroll down
      } else {
        menuIndex = (menuIndex - 1 + 3) % 3;  // Scroll up
      }
      lastPos = newPos;
      displayMenu(menuIndex);
    }

    // Check if the button is pressed to switch to brightness adjustment mode
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(200);  // Debounce delay
      mode = 1;  // Switch to brightness adjustment mode
      lastPos = newPos;
    }
  } 
  // If we are in brightness adjustment mode
  else if (mode == 1) {
    if (newPos != lastPos) {
      int selectedColor = menuIndex;  // RED = 0, GREEN = 1, BLUE = 2
      brightness[selectedColor] = constrain(brightness[selectedColor] + (newPos - lastPos) * 5, 0, 255);  // Adjust brightness
      analogWrite(RED_PIN + selectedColor, brightness[selectedColor]);  // Update LED brightness
      lastPos = newPos;
      displayBrightness(menuIndex, brightness[selectedColor]);
    }

    // Check if the button is pressed to save the brightness and return to the menu
    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(200);  // Debounce delay
      mode = 0;  // Return to menu mode
      displayMenu(menuIndex);  // Display menu again
    }
  }
}

// Function to display the menu
void displayMenu(int index) {
  OLED.firstPage();
  do {
    OLED.setFont(u8g2_font_ncenB08_tr);
    OLED.setCursor(10, 20);
    if (index == 0) OLED.print(">");  // Highlight RED
    OLED.print(" RED");
    OLED.setCursor(10, 30);
    if (index == 1) OLED.print(">");  // Highlight GREEN
    OLED.print(" GREEN");
    OLED.setCursor(10, 40);
    if (index == 2) OLED.print(">");  // Highlight BLUE
    OLED.print(" BLUE");
  } while (OLED.nextPage());
}

// Function to display brightness adjustment
void displayBrightness(int index, int value) {
  OLED.firstPage();
  do {
    OLED.setFont(u8g2_font_ncenB08_tr);
    OLED.setCursor(10, 20);
    if (index == 0) OLED.print("RED");
    if (index == 1) OLED.print("GREEN");
    if (index == 2) OLED.print("BLUE");

    OLED.setCursor(10, 40);
    OLED.print("Brightness: ");
    OLED.print(value);
  } while (OLED.nextPage());
}
