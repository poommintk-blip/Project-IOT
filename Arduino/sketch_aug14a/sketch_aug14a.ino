/*
  LCD 1602 I2C Display Message Test (ESP32) — Robust Init Version
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA   21
#define I2C_SCL   22
#define LCD_ADDR  0x27   // Address 0x27
#define LCD_COLS  16
#define LCD_ROWS  2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int count = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== เริ่มต้นทดสอบการส่งข้อความออกจอ LCD ===");

  // 1. เริ่มต้น I2C และกำหนด Clock ให้อยู่ในโหมด Standard (100kHz)
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // บังคับความเร็ว 100kHz เพื่อเสถียรภาพของชิป PCF8574
  delay(250);

  // 2. Initialized จอ LCD พร้อมเคลียร์ Buffer
  lcd.init();
  lcd.clear();
  delay(100);
  lcd.backlight();
  delay(100);

  // 3. แสดงข้อความต้อนรับ
  lcd.setCursor(0, 0);
  lcd.print("Smart Weather");
  lcd.setCursor(0, 1);
  lcd.print("Lamp System OK!");
  delay(2500);

  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("LCD Live Output ");

  lcd.setCursor(0, 1);
  lcd.print("Runtime: ");
  lcd.print(count);
  lcd.print(" s    ");

  Serial.print("ข้อความบนจอ LCD กำลังแสดง -> Runtime: ");
  Serial.print(count);
  Serial.println(" s");

  count++;
  delay(1000);
}