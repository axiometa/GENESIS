#include <Keyboard.h>

int pin = A7;
char keyPress = 'w';wwwwwwwwwwwwwwwwwww11

int debounceTime = 100;

void setup() {
  pinMode(pin, INPUT_PULLUP);
  Keyboard.begin();
  delay(1000);
}

void loop() {

  if (digitalRead(pin) == 0) {
    Keyboard.press(keyPress);
    delay(debounceTime);
    Keyboard.releaseAll();
  }
}