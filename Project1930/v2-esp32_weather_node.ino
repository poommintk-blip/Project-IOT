/*
  Smart Weather Lamp — Digital Twin (Hardware Node)
  DGT01 1930 โครงงานเครือข่ายไร้สายและอินเทอร์เน็ตของสรรพสิ่ง
  
  การต่อขาที่ใช้งานจริง:
  - DHT22 Data : GPIO14
  - LDR Analog : GPIO34
  - LED น้ำเงิน : GPIO16 (RAIN)
  - LED เหลือง : GPIO18 (NORMAL)
  - LED แดง    : GPIO5  (SUNNY)
  - LCD SDA    : GPIO21
  - LCD SCL    : GPIO22
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- 1. ตั้งค่า Wi-Fi Hotspot มือถือ ----------
// ใส่ชื่อและรหัสผ่าน Hotspot ของโทรศัพท์มือถือ (ต้องเป็นความถี่ 2.4 GHz)
const char* WIFI_SSID     = "MP.A1";
const char* WIFI_PASSWORD = "600058944";

// ---------- 2. กำหนดขา Pin และเซนเซอร์ ----------
#define DHTPIN      14     // DHT DATA Pin (GPIO14)
#define DHTTYPE     DHT22  // DHT22 (AM2302)
#define LDR_PIN     34     // LDR ADC Pin (GPIO34)

#define LED_BLUE    16     // LED น้ำเงิน (RAIN)
#define LED_YELLOW  18     // LED เหลือง (NORMAL)
#define LED_RED     5      // LED แดง (SUNNY)

#define LCD_COLS    16
#define LCD_ROWS    2

// ตั้งเป็น true หากวงจร LDR ต่อสลับฝั่ง (เช่น ตอนมืดค่าพุ่ง 4095 ตอนสว่างค่าลด)
const bool INVERT_LDR = false; 

const float HUMIDITY_RAIN_THRESHOLD = 75.0;
const int   LIGHT_SUNNY_THRESHOLD   = 3000;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C* lcd = nullptr;
WebServer server(80);

String overrideState = "";
String currentState  = "NORMAL";
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000;

float currentTemp = 28.0;
float currentHum  = 60.0;
int   currentLight = 1500;

// ---------- ฟังก์ชันค้นหา I2C Address ของจอ LCD อัตโนมัติ ----------
uint8_t detectLcdAddress() {
  uint8_t addresses[] = {0x27, 0x3F, 0x20, 0x38};
  for (uint8_t addr : addresses) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("[I2C] พบจอ LCD ที่ Address: 0x%02X\n", addr);
      return addr;
    }
  }
  Serial.println("[I2C] ไม่พบจออัตโนมัติ ใช้ค่าเริ่มต้น 0x27");
  return 0x27;
}

// ---------- ฟังก์ชันควบคุมไฟ LED ----------
void updateLeds(const String &state) {
  digitalWrite(LED_BLUE,   state == "RAIN"   ? HIGH : LOW);
  digitalWrite(LED_YELLOW, state == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_RED,    state == "SUNNY"  ? HIGH : LOW);
}

// ---------- ฟังก์ชันแสดงผลบนจอ LCD ----------
void updateLcd(float temp, float humidity, const String &state) {
  if (!lcd) return;
  char line1[17];
  char line2[17];

  snprintf(line1, sizeof(line1), "T:%.1fC H:%.0f%%    ", temp, humidity);
  snprintf(line2, sizeof(line2), "State: %-9s", state.c_str());

  lcd->setCursor(0, 0);
  lcd->print(line1);
  lcd->setCursor(0, 1);
  lcd->print(line2);
}

// ---------- ส่งข้อมูล JSON ออกทาง Web Serial ----------
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

// ---------- REST API Handlers (สำหรับ Mobile App & Web App) ----------
void handleApiTelemetry() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String json = "{\"temp\":" + String(currentTemp, 1) +
                ",\"humidity\":" + String(currentHum, 1) +
                ",\"light\":" + String(currentLight) +
                ",\"state\":\"" + currentState + "\"}";
  server.send(200, "application/json", json);
}

void handleApiSetState() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    mode.toUpperCase();
    if (mode == "RAIN" || mode == "NORMAL" || mode == "SUNNY") {
      overrideState = mode;
      currentState  = mode;
      updateLeds(overrideState);
      updateLcd(currentTemp, currentHum, currentState);
      server.send(200, "text/plain", "OK: " + mode);
      Serial.println("[WIFI 📱 สั่งงาน] สลับสถานะเป็น " + mode);
      return;
    } else if (mode == "AUTO" || mode == "RESET") {
      overrideState = "";
      currentState = classifyWeather(currentHum, currentLight);
      updateLeds(currentState);
      updateLcd(currentTemp, currentHum, currentState);
      server.send(200, "text/plain", "OK: AUTO");
      Serial.println("[WIFI 📱 สั่งงาน] คืนค่าสู่โหมด AUTO");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid Parameter");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  analogReadResolution(12);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);

  // 1. กำหนดค่า I2C และจอ LCD
  Wire.begin(21, 22);
  Wire.setClock(100000);
  delay(200);

  uint8_t lcdAddr = detectLcdAddress();
  lcd = new LiquidCrystal_I2C(lcdAddr, LCD_COLS, LCD_ROWS);

  lcd->init();
  delay(50);
  lcd->backlight();
  delay(50);
  lcd->clear();
  delay(100);

  lcd->setCursor(0, 0);
  lcd->print("Weather Twin    ");
  lcd->setCursor(0, 1);
  lcd->print("Connecting Wi-Fi");

  // 2. เชื่อมต่อ Hotspot มือถือ
  Serial.print("\n[WIFI] กำลังเชื่อมต่อ Hotspot: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WIFI] เชื่อมต่อสำเร็จ!");
    Serial.print("[WIFI] IP Address ของบอร์ด: http://");
    Serial.println(WiFi.localIP());

    lcd->setCursor(0, 0);
    lcd->print("IP:             ");
    lcd->setCursor(3, 0);
    lcd->print(WiFi.localIP());
    lcd->setCursor(0, 1);
    lcd->print("Web Server OK!  ");
  } else {
    Serial.println("\n[WIFI] เชื่อมต่อไม่สำเร็จ รันโหมดออฟไลน์/Serial แทน");
    lcd->setCursor(0, 1);
    lcd->print("Wi-Fi Fail/Local");
  }
  delay(2500);
  lcd->clear();

  // 3. เริ่มต้น REST API Server
  server.on("/api/telemetry", HTTP_GET, handleApiTelemetry);
  server.on("/api/set", HTTP_GET, handleApiSetState);
  server.begin();
  Serial.println("[SERVER] HTTP Web Server เริ่มทำงานบนพอร์ต 80");
}

void loop() {
  // รองรับคำสั่งจาก Web App / โทรศัพท์ผ่าน Wi-Fi
  server.handleClient();

  // 📥 1. รองรับคำสั่งจาก Web Serial Dashboard (สาย USB)
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "RAIN" || input == "NORMAL" || input == "SUNNY") {
      overrideState = input;
      currentState = overrideState;
      updateLeds(overrideState);
      updateLcd(currentTemp, currentHum, currentState);
    } else if (input == "AUTO" || input == "RESET") {
      overrideState = "";
      currentState = classifyWeather(currentHum, currentLight);
      updateLeds(currentState);
      updateLcd(currentTemp, currentHum, currentState);
    }
  }

  // 📤 2. อ่านค่าเซนเซอร์ทุก 2 วินาที
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int lightRaw = analogRead(LDR_PIN);

  if (INVERT_LDR) {
    lightRaw = 4095 - lightRaw;
  }

  if (!isnan(humidity) && !isnan(temp)) {
    currentTemp  = temp;
    currentHum   = humidity;
    currentLight = lightRaw;
  } else {
    Serial.println("{\"error\":\"DHT sensor retry\"}");
  }

  if (overrideState != "") {
    currentState = overrideState;
  } else {
    currentState = classifyWeather(currentHum, currentLight);
  }

  updateLeds(currentState);
  updateLcd(currentTemp, currentHum, currentState);
  sendJson(currentTemp, currentHum, currentLight, currentState);
}