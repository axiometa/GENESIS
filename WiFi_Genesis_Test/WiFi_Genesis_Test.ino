//Port 1 A10, 8
//Port 2 A9,  3
//Port 3 A8,  4
//Port 4 A6,  6
//Port 5 A7,  5
//Port 6 A3,  7
//Port 7 A2,  9
//Port 8 A1,  2
//Port 9 A0,  1

void setup() {
  Serial.begin(9600);
}

void loop() {
  //analogTest();
  //digitalTest();

  relayTest();
}

void digitalTest() {

  //Port 1
  //togglePin(8);
  togglePin(38);
  //togglePin(14);

  //Port 3
  //togglePin(3);
  togglePin(12);
  //togglePin(11);

  //Port 3
  //togglePin(4);
  togglePin(17);
  //togglePin(18);

  // //Port 4
  // togglePin(6);
  // togglePin(0);
  // togglePin(15);

  // //Port 5
  // togglePin(5);
  // togglePin(10);
  // togglePin(13);

  // //Port 6
  // togglePin(7);
  // togglePin(45);
  // togglePin(21);

  //Port 7
  togglePin(9);
  //togglePin(39);
  //togglePin(40);

  //Port 8
  togglePin(2);
  //togglePin(41);
  //togglePin(42);

  //Port 9
  togglePin(1);
  //togglePin(46);
  //togglePin(18);
}


void analogTest() {
  Serial.print(analogRead(8));
  Serial.print(" | ");
  Serial.print(analogRead(3));
  Serial.print(" | ");
  Serial.print(analogRead(4));
  Serial.print(" | ");
  Serial.print(analogRead(6));
  Serial.print(" | ");
  Serial.print(analogRead(5));
  Serial.print(" | ");
  Serial.print(analogRead(7));
  Serial.print(" | ");
  Serial.print(analogRead(9));
  Serial.print(" | ");
  Serial.print(analogRead(2));
  Serial.print(" | ");
  Serial.print(analogRead(1));
  Serial.println(" | ");

  delay(10);
}


void relayTest() {

  pinMode(38, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(1, OUTPUT);

  digitalWrite(38, HIGH);
  digitalWrite(9, HIGH);
  delay(500);
  digitalWrite(38, LOW);
  digitalWrite(9, LOW);
  delay(100);

  digitalWrite(12, HIGH);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(12, LOW);
  digitalWrite(2, LOW);
  delay(100);

  digitalWrite(16, HIGH);
  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(16, LOW);
  digitalWrite(1, LOW);
  delay(100);
}

void togglePin(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(100);
}
