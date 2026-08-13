/*
  Smart Weather Lamp — Digital Twin (Hardware Node)
  DGT01 1930 โครงงานเครือข่ายไร้สายและอินเทอร์เน็ตของสรรพสิ่ง

  === รายการอุปกรณ์ที่ใช้จริง (ตาม BOM) ===
  #26 ESP32 Development Board WiFi Bluetooth   x1  -> Main controller
  #19 DHT11 Temp-Humidity Sensor Module         x1  -> วัดอุณหภูมิ/ความชื้น
  #41 LDR 10-20K 5mm                            x1  -> วัดความเข้มแสง
  #3  LCD 16x2 with I2C Interface               x1
  #4  Arduino I2C Adapter LCD1602/2004 (PCF8574) x1  -> ประกบหลังจอ LCD (ใช้ 4 สาย)
  #36 LED 5mm BLUE   x1  -> สถานะ RAIN
  #37 LED 5mm YELLOW x1  -> สถานะ NORMAL
  #34 LED 5mm RED    x1  -> สถานะ SUNNY
  #30 Resistor 330R  x3  -> คั่นขา LED ทั้ง 3 ดวง
  #31 Resistor 10K   x1  -> ต่อร่วมกับ LDR แบบ Voltage Divider
  #18 Breadboard 830 ช่อง x1
  #29 5V/3.3V Breadboard Power Module x1
  #28 DC 9V 1A Adapter x1
  #45/#46 Jumper wires
  #50 USB A to Micro USB 150cm -> ต่อ ESP32 เข้าคอมพิวเตอร์
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- ขาที่ใช้จริง ----------
#define DHTPIN      4      // DHT DATA Pin (GPIO4)
#define DHTTYPE     DHT11  // ปรับเป็น DHT11 (โมดูลสีฟ้า)
#define LDR_PIN     34     // ADC1_CH6 (input-only)

#define LED_RAIN    16     // สีน้ำเงิน (BLUE)  = ฝนตก
#define LED_NORMAL  17     // สีเหลือง (YELLOW) = แดดปกติ
#define LED_SUNNY   18     // สีแดง   (RED)    = แดดแรง

#define LCD_ADDR    0x27   // I2C Address (0x27 หรือ 0x3F)
#define LCD_COLS    16
#define LCD_ROWS    2

// ---------- เกณฑ์การตัดสินสถานะ ----------
const float HUMIDITY_RAIN_THRESHOLD = 75.0;   // %RH
const int   LIGHT_SUNNY_THRESHOLD   = 3000;   // ค่า ADC (0-4095)

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

String overrideState = ""; // เก็บสถานะจำลองจาก Web Serial
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000; // อ่านค่าทุก 2 วินาที

// ---------- ฟังก์ชันควบคุมส่วนต่างๆ ----------

void updateLeds(const String &state) {
  digitalWrite(LED_RAIN,   state == "RAIN"   ? HIGH : LOW);
  digitalWrite(LED_NORMAL, state == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_SUNNY,  state == "SUNNY"  ? HIGH : LOW);
}

void updateLcd(float temp, float humidity, const String &state) {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("State: ");
  lcd.print(state);
  lcd.print("    ");
}

void sendJson(float temp, float humidity, int lightRaw, const String &state) {
  Serial.print("{\"temp\":");
  Serial.print(temp, 1);
  Serial.print(",\"humidity\":");
  Serial.print(humidity, 1);
  Serial.print(",\"light\":");
  Serial.print(lightRaw);
  Serial.print(",\"state\":\"");
  Serial.print(state);
  Serial.println("\"}");
}

String classifyWeather(float humidity, int lightRaw) {
  if (humidity >= HUMIDITY_RAIN_THRESHOLD) {
    return "RAIN";
  } else if (lightRaw >= LIGHT_SUNNY_THRESHOLD) {
    return "SUNNY";
  } else {
    return "NORMAL";
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  dht.begin();
  analogReadResolution(12); // ESP32: 0-4095

  pinMode(LED_RAIN, OUTPUT);
  pinMode(LED_NORMAL, OUTPUT);
  pinMode(LED_SUNNY, OUTPUT);
  digitalWrite(LED_RAIN, LOW);
  digitalWrite(LED_NORMAL, LOW);
  digitalWrite(LED_SUNNY, LOW);

  Wire.begin(21, 22); // SDA=21, SCL=22
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Twin");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(1500);
  lcd.clear();
}

// ---------- MAIN LOOP ----------
void loop() {
  // 📥 1. อ่านคำสั่งที่ส่งมาจาก Web Serial แบบ Real-time
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "RAIN" || input == "NORMAL" || input == "SUNNY") {
      overrideState = input;
      updateLeds(overrideState); // สั่งไฟเปลี่ยนสีทันทีตามปุ่มบนเว็บ
    } else if (input == "AUTO" || input == "RESET") {
      overrideState = ""; // ยกเลิกการจำลอง กลับไปใช้อัลกอริทึมปกติ
    }
  }

  // 📤 2. อ่านค่าเซนเซอร์ แล้วส่ง JSON ออกไปหาเว็บทุก 2 วินาที
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int lightRaw = analogRead(LDR_PIN);

  // ตรวจสอบความถูกต้องของค่า DHT
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("{\"error\":\"DHT read failed\"}");
    lcd.setCursor(0, 0);
    lcd.print("DHT Read Error ");
    return;
  }

  // เลือกสถานะ (ใช้ค่า override ถ้ามีการสั่งจำลองบนเว็บ หรือคำนวณจริงถ้าไม่มี)
  String state = (overrideState != "") ? overrideState : classifyWeather(humidity, lightRaw);

  updateLeds(state);
  updateLcd(temp, humidity, state);
  sendJson(temp, humidity, lightRaw, state);
}