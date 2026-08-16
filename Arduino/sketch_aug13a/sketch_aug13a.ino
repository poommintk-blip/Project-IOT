/*
  Smart Weather Lamp — Digital Twin (Hardware Node)
  DGT01 1930 โครงงานเครือข่ายไร้สายและอินเทอร์เน็ตของสรรพสิ่ง
  
  การต่อขาที่ใช้งานจริง:
  - DHT11 Data : GPIO4
  - LDR Analog : GPIO34
  - LED น้ำเงิน : GPIO16 (สายเขียว)
  - LED เหลือง : GPIO18 (สายดำ)
  - LED แดง    : GPIO5  (สายแดง)
  - LCD SDA    : GPIO21
  - LCD SCL    : GPIO22
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- ขา Pin ที่ใช้จริง ----------
#define DHTPIN      14      // DHT DATA Pin (GPIO4)
#define DHTTYPE     DHT22   // เปลี่ยนจาก DHT11 เป็น DHT22
#define LDR_PIN     34     // LDR ADC Pin (GPIO34)

#define LED_BLUE    16     // LED สีน้ำเงิน (RAIN)   - GPIO16
#define LED_YELLOW  18     // LED สีเหลือง (NORMAL) - GPIO18
#define LED_RED     5      // LED สีแดง (SUNNY)    - GPIO5

// ปรับ Address ของจอ LCD (ปกติคือ 0x27 หรือ 0x3F)
#define LCD_ADDR    0x27   
#define LCD_COLS    16
#define LCD_ROWS    2

const float HUMIDITY_RAIN_THRESHOLD = 75.0;
const int   LIGHT_SUNNY_THRESHOLD   = 3000;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

String overrideState = "";
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

void updateLeds(const String &state) {
  digitalWrite(LED_BLUE,   state == "RAIN"   ? HIGH : LOW);
  digitalWrite(LED_YELLOW, state == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_RED,    state == "SUNNY"  ? HIGH : LOW);
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

void setup() {
  Serial.begin(115200);
  dht.begin();
  analogReadResolution(12);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weather Twin");
  lcd.setCursor(0, 1);
  lcd.print("Booting...");
  delay(1500);
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

  // 📤 2. อ่านค่าเซนเซอร์และส่งออก Telemetry ทุกๆ 2 วินาที
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int lightRaw = analogRead(LDR_PIN);

  if (isnan(humidity) || isnan(temp)) {
    Serial.println("{\"error\":\"DHT read failed\"}");
    lcd.setCursor(0, 0);
    lcd.print("DHT Read Error ");
    return;
  }

  String state = (overrideState != "") ? overrideState : classifyWeather(humidity, lightRaw);

  updateLeds(state);
  updateLcd(temp, humidity, state);
  sendJson(temp, humidity, lightRaw, state);
}