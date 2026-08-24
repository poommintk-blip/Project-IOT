#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String channelID = "YOUR_CHANNEL_ID"; 

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // ดึงข้อมูล Feed ล่าสุดจาก ThingSpeak
    String url = "http://api.thingspeak.com/channels/" + channelID + "/feeds/last.json";
    http.begin(client, url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      StaticJsonDocument<512> doc;
      deserializeJson(doc, payload);

      float t = doc["field1"].as<float>();
      float h = doc["field2"].as<float>();

      // แสดงผลบนจอ LCD 16x2
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(t, 2);
      lcd.print(" C");

      lcd.setCursor(0, 1);
      lcd.print("Humid: ");
      lcd.print(h, 2);
      lcd.print(" %");
    }
    http.end();
  }
  delay(20000);
}