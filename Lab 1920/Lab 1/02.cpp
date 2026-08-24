// C++ code
//
/*
  ข้อที่ 2: Serial Monitor [2 คะแนน]
  รับข้อมูลนำเข้าจาก Serial Monitor ทีละอักขระ
  แสดงรหัส ASCII ของอักขระที่ได้รับ พร้อมบอกว่าเป็นตัวเลขหรือตัวอักษร
*/

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    Serial.print("I received: ");
    Serial.println((int)c);  // แสดงรหัส ASCII

    if (isDigit(c)) {
      Serial.println("The character is a number");
    } else if (isAlpha(c)) {
      Serial.println("The character is a letter");
    } else {
      Serial.println("The character is a symbol");
    }
  }
}