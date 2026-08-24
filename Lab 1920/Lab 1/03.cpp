// C++ code
//

const int ledPins[] = {2, 3, 4, 5, 6, 7, 8};
const int numLeds = 7;
const int delayTime = 500; // ปรับความเร็วไฟวิ่งได้ที่นี่

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // วิ่งไปทางขวา (2 -> 8)
  for (int i = 0; i < numLeds; i++) {
    lightOnly(i);
    delay(delayTime);
  }

  // วิ่งกลับทางซ้าย (8 -> 2)
  for (int i = numLeds - 2; i >= 0; i--) {
    lightOnly(i);
    delay(delayTime);
  }
}

// เปิดไฟดวงที่ index เดียว ดวงอื่นดับหมด
void lightOnly(int index) {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], (i == index) ? HIGH : LOW);
  }
}