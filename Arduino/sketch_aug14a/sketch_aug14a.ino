/*
  LCD 1602 I2C Display Message Test (ESP32) — Fixed 0x3F
  บัส I2C: SDA=GPIO21, SCL=GPIO22
  Address: 0x3F
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SDA   21
#define I2C_SCL   22
#define LCD_ADDR  0x3F   // Address 0x3F ตรงตามตัวจริง
#define LCD_COLS  16
#define LCD_ROWS  2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int count = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); // หน่วงเวลารอไฟเลี้ยงจอเสถียร

  // 1. เริ่มต้น I2C และกำหนด Clock มาตรฐาน (100kHz)
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  delay(200);

  // 2. Initialized จอ LCD พร้อมเคลียร์ตัวอักษรขยะออก
  lcd.init();
  delay(50);
  lcd.backlight();
  delay(50);
  lcd.clear();
  delay(100);

  // 3. แสดงข้อความต้อนรับ
  lcd.setCursor(0, 0);
  lcd.print("Smart Weather   ");
  lcd.setCursor(0, 1);
  lcd.print("Lamp System OK! ");
  delay(2500);

  lcd.clear();
}

void loop() {
  // แถวที่ 1
  lcd.setCursor(0, 0);
  lcd.print("LCD Live Output ");

  // แถวที่ 2
  lcd.setCursor(0, 1);
  lcd.print("Runtime: ");
  lcd.print(count);
  lcd.print(" s    ");

  Serial.print("Runtime: ");
  Serial.println(count);

  count++;
  delay(1000);
}