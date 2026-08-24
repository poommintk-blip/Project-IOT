const int pirPin = 12;
const int ledPin = 11;
const int motorPin = 3; // ขา PWM
const int tmpPin = A0;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
}

void loop() {
  int pirState = digitalRead(pirPin);
  
  // แปลงค่าแรงดัน Analog เป็นอุณหภูมิ (°C) ของเซนเซอร์ TMP36
  int rawVal = analogRead(tmpPin);
  float voltage = rawVal * (5.0 / 1024.0);
  float temperature = (voltage - 0.5) * 100.0;

  if (pirState == HIGH) { // มีคนอยู่ในห้อง
    digitalWrite(ledPin, HIGH);

    if (temperature > 25.0) {
      // ปรับความเร็วมอเตอร์ (25°C - 45°C แปลงเป็น PWM 80 - 255)
      int motorSpeed = map((int)temperature, 25, 45, 80, 255);
      motorSpeed = constrain(motorSpeed, 80, 255);
      analogWrite(motorPin, motorSpeed);
    } else {
      analogWrite(motorPin, 0);
    }
  } else { // ไม่มีคนอยู่ในห้อง
    digitalWrite(ledPin, LOW);
    analogWrite(motorPin, 0);
  }

  delay(100);
}