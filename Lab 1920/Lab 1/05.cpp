// =============================
// LAB 1 ข้อ 5
// ควบคุมความสว่าง LED
// =============================

int LED = 11;       // LED ต่อ D11 (PWM)
int SW = 10;        // Switch ต่อ D10

// 7-Segment
// a b c d e f g
int seg[] = {2, 3, 4, 5, 6, 7, 8};

// ความสว่าง 0 - 90%
int brightness = 0;

// รูปแบบตัวเลข 0-9
// a b c d e f g
byte number[10][7] = {

  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};


void setup() {

  // LED
  pinMode(LED, OUTPUT);

  // Switch
  pinMode(SW, INPUT_PULLUP);

  // 7-Segment
  for (int i = 0; i < 7; i++) {
    pinMode(seg[i], OUTPUT);
  }

  // เริ่มต้น 0%
  analogWrite(LED, 0);

  // แสดงเลข 0
  showNumber(0);
}


void loop() {

  // ตรวจสอบการกด Switch
  if (digitalRead(SW) == LOW) {

    // เพิ่มความสว่างทีละ 10%
    brightness = brightness + 10;

    // ถ้า 90% แล้วกดอีกครั้ง
    // ให้กลับเป็น 0%
    if (brightness > 90) {
      brightness = 0;
    }

    // แปลง 0-100% เป็น PWM 0-255
    int pwmValue = map(brightness, 0, 100, 0, 255);

    // ควบคุมความสว่าง LED
    analogWrite(LED, pwmValue);

    // แปลงความสว่างเป็นเลข 0-9
    int digit = brightness / 10;

    // แสดงตัวเลขบน 7-Segment
    showNumber(digit);

    // ป้องกันการกดครั้งเดียวแล้วนับหลายครั้ง
    delay(300);
  }
}


// =============================
// ฟังก์ชันแสดงตัวเลข
// =============================

void showNumber(int digit) {

  for (int i = 0; i < 7; i++) {
    digitalWrite(seg[i], number[digit][i]);
  }
}