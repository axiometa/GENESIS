// Pin definitions for LEDs
int led1 = 38;    // Left top
int led2 = 17;   // Right top
int led3 = 0;   // Left centre 
int led4 = 10;   // Centre
int led5 = 45;   // Right centre
int led6 = 39;   // Left bottom
int led7 = 46;  // Right bottom

int buttonPin = 47;  // Pin connected to the button
int buzzerPin = 8;  // Pin connected to the buzzer
int motorPin = 9;   // Pin connected to the vibration motor

void setup() {
  // Set LED pins as output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);  // Centre LED
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);

  // Set button, buzzer, and motor pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(motorPin, OUTPUT);

  // Start with all LEDs off
  turnOffAllLeds();
}

void loop() {
  // Wait for the button to be pressed
  if (digitalRead(buttonPin) == LOW) {
    // Simulate dice rolling with sound and vibration
    simulateRolling();
    
    // Generate a random dice number between 1 and 6
    int diceRoll = random(1, 7);
    
    // Display the dice number on the LEDs
    displayDiceFace(diceRoll);
    
    // Small delay after rolling
    delay(500);
  }
}

void simulateRolling() {
  for (int i = 0; i < 7; i++) {  // Roll the dice 10 times for effect
    int randomFace = random(1, 7);
    displayDiceFace(randomFace);
    
    // Activate buzzer and motor during roll
    tone(buzzerPin, 1000);  // Play a tone
    digitalWrite(motorPin, HIGH);  // Turn on vibration motor
    delay(100);  // Delay to simulate bouncing
    noTone(buzzerPin);  // Stop buzzer
    digitalWrite(motorPin, LOW);  // Turn off motor
    delay(100);  // Short pause between rolls
  }
}

void displayDiceFace(int face) {
  // Turn off all LEDs first
  turnOffAllLeds();

  // Turn on specific LEDs for the dice face
  switch (face) {
    case 1:
      digitalWrite(led4, HIGH); // Center LED for '1'
      break;
    case 2:
      digitalWrite(led1, HIGH); // Left top for '2'
      digitalWrite(led7, HIGH); // Right bottom for '2'
      break;
    case 3:
      digitalWrite(led1, HIGH); // Left top for '3'
      digitalWrite(led4, HIGH); // Center LED for '3'
      digitalWrite(led7, HIGH); // Right bottom for '3'
      break;
    case 4:
      digitalWrite(led1, HIGH); // Left top for '4'
      digitalWrite(led2, HIGH); // Right top for '4'
      digitalWrite(led6, HIGH); // Left bottom for '4'
      digitalWrite(led7, HIGH); // Right bottom for '4'
      break;
    case 5:
      digitalWrite(led1, HIGH); // Left top for '5'
      digitalWrite(led2, HIGH); // Right top for '5'
      digitalWrite(led4, HIGH); // Center LED for '5'
      digitalWrite(led6, HIGH); // Left bottom for '5'
      digitalWrite(led7, HIGH); // Right bottom for '5'
      break;
    case 6:
      digitalWrite(led1, HIGH); // Left top for '6'
      digitalWrite(led3, HIGH); // Left center for '6'
      digitalWrite(led2, HIGH); // Right top for '6'
      digitalWrite(led5, HIGH); // Right center for '6'
      digitalWrite(led6, HIGH); // Left bottom for '6'
      digitalWrite(led7, HIGH); // Right bottom for '6'
      break;
  }
}

void turnOffAllLeds() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);  // Centre LED
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(led7, LOW);
}
