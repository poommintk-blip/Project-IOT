#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------- ปรับตามการต่อสายจริง ----------
#define DHTPIN      4      // DHT22 data pin (ESP32: GPIO4 / NodeMCU: D2)
#define DHTTYPE     DHT22
#define LDR_PIN     34     // ESP32 ADC pin (ใช้ A0 บน NodeMCU)

#define LED_RAIN    16     // สีน้ำเงิน = ฝนตก
#define LED_NORMAL  17     // สีส้ม/เหลือง = แดดปกติ
#define LED_SUNNY   18     // สีแดง = แดดแรง

#define LCD_ADDR    0x27
#define LCD_COLS    16
#define LCD_ROWS    2

// ---------- เกณฑ์การตัดสินสถานะ (ปรับตามค่าจริงที่วัดได้ในพื้นที่) ----------
const float HUMIDITY_RAIN_THRESHOLD = 75.0;   // %RH สูงกว่านี้ถือว่ามีโอกาสฝนตก
const int   LIGHT_SUNNY_THRESHOLD   = 3000;   // ค่า ADC (0-4095 บน ESP32) สูงกว่านี้ถือว่าแดดแรง
const int   LIGHT_NORMAL_THRESHOLD  = 600;    // ต่ำกว่านี้ถือว่ามืด/ไม่มีแดด

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

String lastState = "";
unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2000; // อ่านค่าทุก 2 วินาที

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_RAIN, OUTPUT);
  pinMode(LED_NORMAL, OUTPUT);
  pinMode(LED_SUNNY, OUTPUT);
  digitalWrite(LED_RAIN, LOW);
  digitalWrite(LED_NORMAL, LOW);
  digitalWrite(LED_SUNNY, LOW);

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
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  int lightRaw = analogRead(LDR_PIN);

  if (isnan(humidity) || isnan(temp)) {
    Serial.println("ERR: DHT22 read failed");
    lcd.setCursor(0, 0);
    lcd.print("Sensor error!  ");
    return;
  }

  String state = classifyWeather(humidity, lightRaw);
  updateLeds(state);
  updateLcd(temp, humidity, lightRaw, state);

  // ส่งสถานะออกทาง Serial เฉพาะตอนเปลี่ยนแปลง เพื่อลดสัญญาณรบกวนบนหน้าเว็บ
  if (state != lastState) {
    Serial.println(state);
    lastState = state;
  }

  // ส่งค่าดิบไว้ debug เพิ่มเติม (หน้าเว็บจะข้ามบรรทัดที่ไม่ตรงคำสั่ง)
  Serial.print("DEBUG T=");
  Serial.print(temp);
  Serial.print("C H=");
  Serial.print(humidity);
  Serial.print("% L=");
  Serial.println(lightRaw);
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

void updateLeds(const String &state) {
  digitalWrite(LED_RAIN,   state == "RAIN"   ? HIGH : LOW);
  digitalWrite(LED_NORMAL, state == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_SUNNY,  state == "SUNNY"  ? HIGH : LOW);
}

void updateLcd(float temp, float humidity, int lightRaw, const String &state) {
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
