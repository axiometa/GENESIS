//https://pulsesensor.com/pages/code-and-guide
#include <U8g2lib.h>
#include <PulseSensorPlayground.h>

PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"


int PulseSensorPurplePin = A0;  // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED = LED_BUILTIN;          //  The on-board Arduion LED

int Signal;           // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 8275;  // Determine which Signal to "count as a beat", and which to ingore.

U8G2_SSD1306_128X64_NONAME_F_HW_I2C OLED(U8G2_R0, U8X8_PIN_NONE);  // Main time display

// The SetUp Function:
void setup() {
  pinMode(LED, OUTPUT);  // pin that will blink to your heartbeat!
  Serial.begin(9600);    // Set's up Serial Communication at certain speed.

  OLED.begin();
  OLED.setFont(u8g2_font_logisoso42_tn);
  OLED.clearDisplay();

  analogReadResolution(14);

  pulseSensor.analogInput(PulseSensorPurplePin);
  pulseSensor.blinkOnPulse(LED);  //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.
  }
}

// The Main Loop Function
void loop() {

  if (pulseSensor.sawStartOfBeat()) {              // Constantly test to see if "a beat happened".
    int myBPM = pulseSensor.getBeatsPerMinute();   // Calls function on our pulseSensor object that returns BPM as an "int".
                                                   // "myBPM" hold this BPM value now.
    Serial.println("♥  A HeartBeat Happened ! ");  // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                         // Print phrase "BPM: "
    Serial.println(myBPM);
    displayRate(myBPM);
  }
  delay(20);
}


void displayRate(int heartRate) {
  OLED.clearBuffer();

  // Draw a pulsating heart icon
  int heartSize = map(heartRate, 60, 120, 8, 12);  // Subtle size changes for pulsation
  drawHeart(20, 32, heartSize);

  // Display the heart rate value next to the heart
  OLED.setCursor(40, 38);                 // Position right next to the heart
  OLED.setFont(u8g2_font_logisoso22_tn);  // Smaller font for better alignment
  OLED.print(heartRate);
  OLED.print(" BPM");

  OLED.sendBuffer();
}

void drawHeart(int x, int y, int size) {
  // Draw a small pulsating heart shape
  OLED.drawCircle(x - size / 4, y - size / 4, size / 4);                 // Left semi-circle
  OLED.drawCircle(x + size / 4, y - size / 4, size / 4);                 // Right semi-circle
  OLED.drawTriangle(x - size / 2, y, x + size / 2, y, x, y + size / 2);  // Bottom triangle
}
