// Pin definitions
const int buttonA0 = A10;  // User A0
const int button17 = A2;  // User 17
const int buttonA9 = 4;       // User A9
const int buttonA10 = 4;      // User A10

const int ledA1 = 12;  // LED for User A0
const int led2 = 1;    // LED for User 17
const int led7 = 4;    // LED for User A9
const int ledA2 = 4;  // LED for User A10

const int buzzer = 8;     // Buzzer pin
const int ledFire1 = 10;  // LED for anticipation

// Game variables
bool gameRunning = false;
bool winnerDetected = false;
bool disqualifiedA0 = false;
bool disqualified17 = false;
bool disqualifiedA9 = false;
bool disqualifiedA10 = false;

void setup() {
  pinMode(buttonA0, INPUT_PULLUP);
  pinMode(button17, INPUT_PULLUP);
  pinMode(buttonA9, INPUT_PULLUP);
  pinMode(buttonA10, INPUT_PULLUP);

  pinMode(ledA1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(ledA2, OUTPUT);

  pinMode(buzzer, OUTPUT);
  pinMode(ledFire1, OUTPUT);

  // Turn off all LEDs
  digitalWrite(ledA1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led7, LOW);
  digitalWrite(ledA2, LOW);
}

void loop() {
  if (!gameRunning) {
    startGame();
  }

  // Check for winner
  if (!winnerDetected && gameRunning) {
    checkForWinner();
  }
}

void startGame() {
  // Buzzer builds anticipation
  for (int i = 0; i < 5; i++) {
    tone(buzzer, 1000);  // Beep
    delay(100);
    noTone(buzzer);  // Silence
    delay(100);
  }

  // Random delay before anticipation LEDs fire up
  int randomDelay = random(1000, 5000);
  delayWithPrematurePressCheck(randomDelay);  // Check for premature press during delay

  // Fire up anticipation LEDs
  digitalWrite(ledFire1, HIGH);

  gameRunning = true;
  winnerDetected = false;
}

void delayWithPrematurePressCheck(int delayDuration) {
  int startTime = millis();
  while (millis() - startTime < delayDuration) {
    checkForPrematurePress();  // Check for premature presses during the delay
  }
}

void checkForPrematurePress() {
  // If any button is pressed before LEDs turn on, disqualify the player
  if (digitalRead(buttonA0) == LOW && !disqualifiedA0) {
    disqualifyPlayer(ledA1, "Player A0");
    disqualifiedA0 = true;
  } else if (digitalRead(button17) == LOW && !disqualified17) {
    disqualifyPlayer(led2, "Player 17");
    disqualified17 = true;
  } else if (digitalRead(buttonA9) == LOW && !disqualifiedA9) {
    disqualifyPlayer(led7, "Player A9");
    disqualifiedA9 = true;
  } else if (digitalRead(buttonA10) == LOW && !disqualifiedA10) {
    disqualifyPlayer(ledA2, "Player A10");
    disqualifiedA10 = true;
  }
}

void checkForWinner() {
  // If any button is pressed and the player is not disqualified, they win the game
  if (digitalRead(buttonA0) == LOW && !disqualifiedA0) {
    announceWinner(ledA1);
  } else if (digitalRead(button17) == LOW && !disqualified17) {
    announceWinner(led2);
  } else if (digitalRead(buttonA9) == LOW && !disqualifiedA9) {
    announceWinner(led7);
  } else if (digitalRead(buttonA10) == LOW && !disqualifiedA10) {
    announceWinner(ledA2);
  }
}

void disqualifyPlayer(int disqualifiedLed, const char* player) {
  // Flash the player's LED and beep to indicate disqualification
  digitalWrite(disqualifiedLed, HIGH);
  tone(buzzer, 500, 200);  // Lower tone for disqualification
  delay(200);
  digitalWrite(disqualifiedLed, LOW);
  delay(200);
  digitalWrite(disqualifiedLed, HIGH);
  delay(200);
  digitalWrite(disqualifiedLed, LOW);
}

void announceWinner(int winnerLed) {
  // Turn off anticipation LEDs
  digitalWrite(ledFire1, LOW);

  // Light up the winner's LED
  digitalWrite(winnerLed, HIGH);

  // Play winning sound
  tone(buzzer, 2000, 500);

  winnerDetected = true;
  delay(3000);  // Show winner for 3 seconds
  resetGame();
}

void resetGame() {
  // Turn off all LEDs
  digitalWrite(ledA1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led7, LOW);
  digitalWrite(ledA2, LOW);

  // Reset game variables
  gameRunning = false;
  winnerDetected = false;
  disqualifiedA0 = false;
  disqualified17 = false;
  disqualifiedA9 = false;
  disqualifiedA10 = false;
}
