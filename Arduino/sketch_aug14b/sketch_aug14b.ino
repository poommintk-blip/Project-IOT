/*
  DHT Sensor Diagnostic Test (ESP32)
  ทดสอบอ่านค่าอุณหภูมิและความชื้นแบบ Real-time
*/

#include <DHT.h>

#define DHTPIN      14      // ขา DATA ของ DHT ต่อเข้า GPIO4
#define DHTTYPE     DHT22  // โมดูลตัวถังสีฟ้า (ถ้าใช้ตัวถังสีขาวให้เปลี่ยนเป็น DHT22)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n==========================================");
  Serial.println("       DHT Sensor Diagnostic Tool         ");
  Serial.println("==========================================");
  
  dht.begin();
  Serial.println("กำลังเริ่มอ่านค่าจากเซนเซอร์ DHT (รอ 2 วินาที)...");
}

void loop() {
  // เซนเซอร์ DHT ต้องการเวลาหน่วงในการอ่านค่าอย่างน้อย 2 วินาที
  delay(2000);

  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();

  // ตรวจสอบสถานะการอ่านค่า
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("❌ ไม่สามารถอ่านค่าได้! (DHT Read Failed)");
    Serial.println("  -> ตรวจสอบ:");
    Serial.println("     1. สาย DATA เสียบตรงขา GPIO14 แน่นหนาดีหรือไม่");
    Serial.println("     2. สายไฟเลี้ยง VCC (3.3V/5V) และ GND แน่นหรือไม่");
    Serial.println("     3. กำหนดประเภท DHT11/DHT22 ถูกต้องหรือไม่\n");
    return;
  }

  // แสดงผลค่าที่อ่านได้สำเร็จ
  Serial.print("✅ อ่านค่าสำเร็จ -> ");
  Serial.print("อุณหภูมิ: ");
  Serial.print(tempC, 1);
  Serial.print(" °C | ความชื้นสัมพัทธ์: ");
  Serial.print(humidity, 1);
  Serial.println(" %RH");
}