const int potPin = A1;
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8};
const int numLeds = 7;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  int sensorValue = analogRead(potPin);
  
  // แปลงค่าช่วง 0-1023 ให้เป็นจำนวนหลอดที่ต้องติด (0 ถึง 7 หลอด)
  int ledsToLight = map(sensorValue, 0, 1023, 0, numLeds);

  for (int i = 0; i < numLeds; i++) {
    if (i < ledsToLight) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
  delay(20);
}