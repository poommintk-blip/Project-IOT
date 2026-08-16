/*
  LED Chaser Test — Smart Weather Lamp
  ทดสอบการทำงานของไฟ LED 3 สี (วิ่งวนเรียงลำดับ)
  ขาที่ใช้จริง: GPIO2, GPIO4, GPIO5
*/

// ---------- กำหนดขา Pin ตามที่คุณย้ายมา ----------
#define LED_BLUE    16   // เปลี่ยนเป็นขาที่สายสีเขียวเสียบอยู่
#define LED_YELLOW  18   // เปลี่ยนเป็นขาที่สายสีดำเสียบอยู่
#define LED_RED     5   // เปลี่ยนเป็นขาที่สายสีแดงเสียบอยู่
// ระยะเวลาหน่วงไฟแต่ละดวง (มิลลิวินาที)
const int DELAY_TIME = 1000; // 0.5 วินาที

void setup() {
  Serial.begin(115200);
  Serial.println("=== เริ่มการทดสอบไฟ LED วิ่งวน (Pin 2, 4, 5) ===");

  // กำหนดสถานะให้ Pin เป็น OUTPUT
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // ปิดไฟทุกดวงก่อนเริ่มต้น
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

void loop() {
  // 1. ติดไฟสีน้ำเงิน (GPIO2)
  Serial.println("1. LED BLUE ON (GPIO16)");
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(DELAY_TIME);

// 2. ติดไฟสีเหลือง (GPIO4)
  Serial.println("2. LED YELLOW ON (GPIO18)");
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  delay(DELAY_TIME);

  // 3. ติดไฟสีแดง (GPIO5)
  Serial.println("3. LED RED ON (GPIO5)");
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  delay(DELAY_TIME);
}