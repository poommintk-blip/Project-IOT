#include <Arduino.h> // <--- ใส่บรรทัดนี้ไว้บนสุด
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ... โค้ดส่วนที่เหลือเหมือนเดิม ...// 1. กำหนดขา Pin ให้ตรงตามวงจร
const int LED_BLUE   = 16;
const int LED_YELLOW = 17;
const int LED_RED    = 18;

void setup() {
  // ตั้งค่า Baud Rate ให้ตรงกับหน้าเว็บ
  Serial.begin(115200);

  // กำหนดสถานะให้ Pin LED เป็น OUTPUT
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void loop() {
  // ----------------------------------------------------
  // 📥 ส่วนรับค่าคำสั่งจาก Web Serial เพื่อควบคุมไฟ LED
  // ----------------------------------------------------
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input == "RAIN") {
      digitalWrite(LED_BLUE, HIGH);   // เปิดไฟน้ำเงิน
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
    } else if (input == "NORMAL") {
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_YELLOW, HIGH); // เปิดไฟเหลือง
      digitalWrite(LED_RED, LOW);
    } else if (input == "SUNNY") {
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, HIGH);   // เปิดไฟแดง
    }
  }

  // ----------------------------------------------------
  // 📤 ส่วนอ่านค่าเซนเซอร์ แล้วส่ง JSON ออกไปหาเว็บ (มีอยู่แล้ว)
  // ----------------------------------------------------
  // (โค้ดอ่านค่า DHT22 / LDR แล้ว Serial.println JSON) ...
}