// C++ code
/*
  ข้อที่ 4: ปุ่มควบคุมไฟ [3 คะแนน]
  LED 6 ดวง ต่อที่ Digital 4 - Digital 9
  Switch 1 ต่อที่ Digital 2 : กดแล้ว LED ติดเพิ่มขึ้นทีละดวง (4 -> 9) สูงสุด 6 ครั้ง
  Switch 2 ต่อที่ Digital 3 : กดแล้ว LED ดับลงทีละดวง เรียงจากดวงที่ไฟติดสูงสุดไปน้อย
*/

const int ledPins[] = {4, 5, 6, 7, 8, 9};
const int numLeds = 6;

const int switch1Pin = 2; // ปุ่มเพิ่มไฟ
const int switch2Pin = 3; // ปุ่มลดไฟ

int ledCount = 0; // จำนวน LED ที่ติดอยู่ ณ ขณะนี้ (0 - 6)

int lastSwitch1State = LOW;
int lastSwitch2State = LOW;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(switch1Pin, INPUT);
  pinMode(switch2Pin, INPUT);

  updateLeds();
}

void loop() {
  int switch1State = digitalRead(switch1Pin);
  int switch2State = digitalRead(switch2Pin);

  // ตรวจจับขอบขาขึ้นของปุ่ม (กดครั้งเดียวนับครั้งเดียว)
  if (switch1State == HIGH && lastSwitch1State == LOW) {
    if (ledCount < numLeds) {
      ledCount++;
      updateLeds();
    }
    delay(50); // กันการเด้งของปุ่ม (debounce)
  }

  if (switch2State == HIGH && lastSwitch2State == LOW) {
    if (ledCount > 0) {
      ledCount--;
      updateLeds();
    }
    delay(50);
  }

  lastSwitch1State = switch1State;
  lastSwitch2State = switch2State;
}

// เปิด LED ตั้งแต่ Digital 4 ขึ้นไปตามจำนวน ledCount ดวงแรก
void updateLeds() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], (i < ledCount) ? HIGH : LOW);
  }
}