#define LIGHT1_RED A8
#define LIGHT1_YELLOW 16
#define LIGHT1_GREEN 15

#define LIGHT2_RED A0
#define LIGHT2_YELLOW 9
#define LIGHT2_GREEN 8

#define LED A10
#define BUTTON A2
#define LDR A6

int ldrThreshold = 500; // Adjust threshold as needed
bool ledState = false;  // Initial state of LED
unsigned long previousMillis = 0;
const int blinkInterval = 500; // Interval for blinking green light

void setup() {
  // Traffic light pins
  pinMode(LIGHT1_RED, OUTPUT);
  pinMode(LIGHT1_YELLOW, OUTPUT);
  pinMode(LIGHT1_GREEN, OUTPUT);

  pinMode(LIGHT2_RED, OUTPUT);
  pinMode(LIGHT2_YELLOW, OUTPUT);
  pinMode(LIGHT2_GREEN, OUTPUT);

  // LED and Button pins
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
}

void loop() {
  static bool light1Green = true; // Track which light is green
  static bool buttonPressed = false;
  int ldrValue = analogRead(LDR);

  // Toggle LED based on LDR
  if (ldrValue < ldrThreshold ) {
    ledState = true;
    digitalWrite(LED, HIGH);
  } else if (ldrValue > ldrThreshold) {
    ledState = false;
    digitalWrite(LED, LOW);
  }

  // Button press logic
  if (digitalRead(BUTTON) == LOW && !buttonPressed) {
    buttonPressed = true; // Prevent repeated actions during a single press
    delay(50); // Debounce

    // Handle traffic light sequence
    if (light1Green) {
      blinkGreen(LIGHT1_GREEN);
      transitionToRed(LIGHT1_GREEN, LIGHT1_YELLOW, LIGHT1_RED);
      transitionToGreen(LIGHT2_RED, LIGHT2_YELLOW, LIGHT2_GREEN);
      delay(3000); // Stay green for 3 seconds
      transitionToRed(LIGHT2_GREEN, LIGHT2_YELLOW, LIGHT2_RED);
    } else {
      blinkGreen(LIGHT2_GREEN);
      transitionToRed(LIGHT2_GREEN, LIGHT2_YELLOW, LIGHT2_RED);
      transitionToGreen(LIGHT1_RED, LIGHT1_YELLOW, LIGHT1_GREEN);
      delay(3000); // Stay green for 3 seconds
      transitionToRed(LIGHT1_GREEN, LIGHT1_YELLOW, LIGHT1_RED);
    }

    light1Green = !light1Green; // Toggle light status
    buttonPressed = false;
  }
}

void blinkGreen(int greenPin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(greenPin, HIGH);
    delay(blinkInterval);
    digitalWrite(greenPin, LOW);
    delay(blinkInterval);
  }
}

void transitionToRed(int greenPin, int yellowPin, int redPin) {
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, HIGH);
  delay(1000); // Yellow light duration
  digitalWrite(yellowPin, LOW);
  digitalWrite(redPin, HIGH);
}

void transitionToGreen(int redPin, int yellowPin, int greenPin) {
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  delay(1000); // Yellow light duration
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, HIGH);
}
