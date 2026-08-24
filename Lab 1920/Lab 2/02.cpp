const int trigPin = 10;
const int echoPin = 9;
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8};
const int numLeds = 7;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // ส่งสัญญาณ Ultrasonic Pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // คำนวณระยะทางเป็นเซนติเมตร
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  int ledsToLight = 0;

  if (distance > 120) {
    ledsToLight = 0;
  } else if (distance >= 110) {
    ledsToLight = 1;
  } else if (distance >= 100) {
    ledsToLight = 2;
  } else if (distance >= 90) {
    ledsToLight = 3;
  } else if (distance >= 80) {
    ledsToLight = 4;
  } else if (distance >= 70) {
    ledsToLight = 5;
  } else if (distance >= 60) {
    ledsToLight = 6;
  } else {
    ledsToLight = 7; // ต่ำกว่า 60 ซม. ติดทั้ง 7 หลอด
  }

  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], (i < ledsToLight) ? HIGH : LOW);
  }

  delay(60);
}