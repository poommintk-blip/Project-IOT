/*
  Smart Weather Lamp — Digital Twin (Hardware Node)
  DGT01 1930 โครงงานเครือข่ายไร้สายและอินเทอร์เน็ตของสรรพสิ่ง
  
  การต่อขาที่ใช้งานจริง:
  - DHT Data   : GPIO14
  - LDR Analog : GPIO34
  - LED น้ำเงิน : GPIO16 (RAIN)
  - LED เหลือง : GPIO18 (NORMAL)
  - LED แดง    : GPIO5  (SUNNY)
  - LCD SDA    : GPIO21
  - LCD SCL    : GPIO22
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- กำหนดขา Pin และการตั้งค่า ----------
#define DHTPIN      14     // DHT DATA Pin (GPIO14)
#define DHTTYPE     DHT22  // ชนิดโมดูล DHT
#define LDR_PIN     34     // LDR ADC Pin (GPIO34)

#define LED_BLUE    16     // LED สีน้ำเงิน (RAIN)
#define LED_YELLOW  18     // LED สีเหลือง (NORMAL)
#define LED_RED     5      // LED สีแดง (SUNNY)

// ปรับ Address เป็น 0x3F ตามที่จอจริงตอบสนอง
#define LCD_ADDR    0x3F   
#define LCD_COLS    16
#define LCD_ROWS    2

const float HUMIDITY_RAIN_THRESHOLD = 75.0;
const int   LIGHT_SUNNY_THRESHOLD   = 3000;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

String overrideState = "";
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

float lastValidTemp = 28.0;
float lastValidHum  = 60.0;

// ---------- ฟังก์ชันควบคุมไฟ LED ----------
void updateLeds(const String &state) {
  digitalWrite(LED_BLUE,   state == "RAIN"   ? HIGH : LOW);
  digitalWrite(LED_YELLOW, state == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_RED,    state == "SUNNY"  ? HIGH : LOW);
}

// ---------- ฟังก์ชันแสดงผลบนจอ LCD ----------
void updateLcd(float temp, float humidity, const String &state) {
  char line1[17];
  char line2[17];

  // แถวที่ 1: แสดงอุณหภูมิและความชื้น
  snprintf(line1, sizeof(line1), "T:%.1fC H:%.0f%%    ", temp, humidity);
  
  // แถวที่ 2: แสดงสถานะสภาพอากาศ
  snprintf(line2, sizeof(line2), "State: %-9s", state.c_str());

  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// ---------- ส่งข้อมูล JSON ออกทาง Serial ----------
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

// ---------- วิเคราะห์สภาพอากาศ ----------
String classifyWeather(float humidity, int lightRaw) {
  if (humidity >= HUMIDITY_RAIN_THRESHOLD) {
    return "RAIN";
  } else if (lightRaw >= LIGHT_SUNNY_THRESHOLD) {
    return "SUNNY";
  } else {
    return "NORMAL";
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); // หน่วงเวลารอแรงดันไฟนิ่ง

  dht.begin();
  analogReadResolution(12);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // ตั้งค่า I2C และจอ LCD (เคลียร์สัญญาณขยะตอนบูต)
  Wire.begin(21, 22);
  Wire.setClock(100000);
  delay(200);

  lcd.init();
  delay(50);
  lcd.backlight();
  delay(50);
  lcd.clear();
  delay(100);

  lcd.setCursor(0, 0);
  lcd.print("Weather Twin    ");
  lcd.setCursor(0, 1);
  lcd.print("Booting...      ");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 📥 1. รับคำสั่งจำลองสถานะจาก Web Serial Dashboard
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "RAIN" || input == "NORMAL" || input == "SUNNY") {
      overrideState = input;
      updateLeds(overrideState);
    } else if (input == "AUTO" || input == "RESET") {
      overrideState = "";
    }
  }

  // 📤 2. อ่านค่าเซนเซอร์และส่งข้อมูลออกทุก 2 วินาที
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int lightRaw = analogRead(LDR_PIN);

  String state = "";

  if (overrideState != "") {
    state = overrideState;
    if (isnan(humidity)) humidity = (state == "RAIN") ? 85.0 : ((state == "NORMAL") ? 55.0 : 30.0);
    if (isnan(temp))     temp     = (state == "RAIN") ? 24.0 : ((state == "NORMAL") ? 29.5 : 36.0);
  } else {
    if (isnan(humidity) || isnan(temp)) {
      // ดึงค่าล่าสุดมาแสดงแทนเพื่อไม่ให้จอค้างตัวอักษรขยะ
      temp = lastValidTemp;
      humidity = lastValidHum;
      Serial.println("{\"error\":\"DHT read retry\"}");
    } else {
      lastValidTemp = temp;
      lastValidHum = humidity;
    }
    state = classifyWeather(humidity, lightRaw);
  }

  updateLeds(state);
  updateLcd(temp, humidity, state);
  sendJson(temp, humidity, lightRaw, state);
}