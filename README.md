# Smart Weather Lamp — Digital Twin IoT Project

ระบบโคมไฟตรวจวัดสภาพแวดล้อมอัจฉริยะและแบบจำลองสภาพอากาศดิจิทัลทวิน (Digital Twin) แบบเรียลไทม์ รองรับการมอนิเตอร์และสั่งการทำงาน 2 โหมด: ควบคุมผ่านสาย USB บนเครื่องคอมพิวเตอร์ด้วย **Web Serial API** และควบคุมแบบไร้สายผ่าน Wi-Fi Hotspot ด้วย **Android Web App (WebView)**

---

## 🌟 ฟีเจอร์หลัก (Key Features)

* **Real-time Live Telemetry:** ตรวจวัดอุณหภูมิ (Temperature), ความชื้นสัมพัทธ์ (Humidity) และค่าความเข้มแสง (Light/ADC) แบบเรียลไทม์


* **Dynamic Digital Twin Particle Scene:** แอนิเมชันจำลองสภาพแวดล้อมด้วย HTML5 Canvas แบบเรียลไทม์ (ละอองหมอกความชื้น, แดดร่มพร้อมลำแสงหมุน, เปลวความร้อนเรืองแสง)
* **Cross-Platform Connectivity:**
* **Web Serial API (PC):** เชื่อมต่อสาย USB ควบคุมและรับ Data Stream โดยตรงผ่านเบราว์เซอร์


* **Android Application (Mobile):** รันบนสมาร์ตโฟนผ่าน WebView โดยดึงข้อมูลและส่งคำสั่งผ่าน REST API (HTTP) ของบอร์ด ESP32


* **Dynamic Override & Simulation:** รองรับโหมดจำลองสภาพอากาศ (HUMID / NORMAL / SUNNY) และคืนค่าสู่โหมดอ่านเซนเซอร์จริงอัตโนมัติ (AUTO)
* **Status Indicator:** ซิงค์ไฟสถานะทั้งบนหน้า Dashboard (LED Bulbs) และหลอด LED จริงบนฮาร์ดแวร์



---

## 🛠️ รายการอุปกรณ์ฮาร์ดแวร์ (Hardware Components)

| อุปกรณ์ | ขาที่เชื่อมต่อ ESP32 (Pin) | คำอธิบาย |
| --- | --- | --- |
| **ESP32 NodeMCU** | — | บอร์ดประมวลผลหลัก (Wi-Fi 2.4 GHz + REST Server) |
| **DHT22** | `GPIO14` | เซนเซอร์ตรวจวัดอุณหภูมิและความชื้นสัมพัทธ์ |
| **LDR Sensor** | `GPIO34` (ADC) | ตัวต้านทานไวแสง (วงจรแบ่งแรงดัน Voltage Divider) |
| **LED น้ำเงิน (HUMID)** | `GPIO16` | ไฟแสดงสถานะความชื้นสูง |
| **LED เหลือง (NORMAL)** | `GPIO18` | ไฟแสดงสถานะสภาพอากาศปกติ |
| **LED แดง (SUNNY)** | `GPIO5` | ไฟแสดงสถานะแดดแรง/อุณหภูมิสูง |
| **LCD 1602 I2C** | `SDA: GPIO21` / `SCL: GPIO22` | จอแสดงผลค่าเซนเซอร์และสถานะแบบตัวอักษร |

---

## 📁 โครงสร้างโปรเจกต์ (Project Structure)

```text
.
├── android/                         # ซอร์สโค้ดฝั่ง Android Studio Project
│   ├── app/
│   │   ├── src/main/
│   │   │   ├── assets/
│   │   │   │   └── index.html       # หน้าเว็บแดชบอร์ด Digital Twin และ Canvas Engine
│   │   │   ├── java/.../
│   │   │   │   └── MainActivity.java # คลาสตั้งค่า WebView
│   │   │   ├── res/layout/
│   │   │   │   └── activity_main.xml # Layout หน้าจอแอป
│   │   │   └── AndroidManifest.xml   # กำหนดสิทธิ์ Network และ UsesCleartextTraffic
├── esp32/
│   └── smart_weather_lamp.ino       # ซอร์สโค้ดเฟิร์มแวร์ Arduino สำหรับบอร์ด ESP32
└── README.md

```

---

## 🚀 ขั้นตอนการติดตั้งและเริ่มใช้งาน (Getting Started)

### 1. ฝั่งฮาร์ดแวร์และเฟิร์มแวร์ (ESP32)

1. ติดตั้งไลบรารีใน Arduino IDE:
* `DHT sensor library` (Adafruit)
* `LiquidCrystal_I2C`


2. เปิดไฟล์ `smart_weather_lamp.ino` แล้วกำหนดค่า Wi-Fi Hotspot:
```cpp
const char* WIFI_SSID     = "YOUR_HOTSPOT_NAME";
const char* WIFI_PASSWORD = "YOUR_HOTSPOT_PASSWORD";

```


3. ต่อวงจรเซนเซอร์ตามตาราง Pinout ด้านบน
4. อัปโหลดโค้ดลงบอร์ด ESP32 จากนั้นเปิด Serial Monitor (Baud rate `115200`) หรือสังเกตที่หน้าจอ LCD เพื่อจดจำหมายเลข IP Address (เช่น `192.168.1.6`)

### 2. ฝั่งแอปพลิเคชัน (Android Studio)

1. เปิดโปรเจกต์ด้วย Android Studio
2. นำไฟล์ `index.html` วางไว้ในโฟลเดอร์ `app/src/main/assets/`
3. ตรวจสอบไฟล์ `AndroidManifest.xml` ให้มีสิทธิ์เชื่อมต่อเครือข่ายและรองรับ HTTP:
```xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<application android:usesCleartextTraffic="true" ...>

```


4. บิลด์และติดตั้งแอปลงสมาร์ตโฟน (เปิด Hotspot บนมือถือเพื่อให้บอร์ด ESP32 เกาะเครือข่ายเดียวกัน)
5. เปิดแอป กรอกหมายเลข IP Address ของ ESP32 ลงในช่อง **ESP32 IP** แล้วกด **บันทึก IP**

---

## 📡 REST API Specifications (ESP32)

* **ดึงข้อมูล Telemetry:**
* `GET /api/telemetry`
* **Response (JSON):**
```json
{
  "temp": 28.7,
  "humidity": 73.0,
  "light": 304,
  "state": "NORMAL"
}

```




* **สั่งเปลี่ยนสถานะสภาพแวดล้อม:**
* `GET /api/set?mode={MODE}`
* พารามิเตอร์ `{MODE}`: `HUMID` | `NORMAL` | `SUNNY` | `AUTO`
