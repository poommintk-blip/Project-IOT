if (Serial.available() > 0) {
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  if (input == "RAIN") {
    digitalWrite(16, HIGH); // LED BLUE
    digitalWrite(17, LOW);  // LED YELLOW
    digitalWrite(18, LOW);  // LED RED
  } else if (input == "NORMAL") {
    digitalWrite(16, LOW);
    digitalWrite(17, HIGH);
    digitalWrite(18, LOW);
  } else if (input == "SUNNY") {
    digitalWrite(16, LOW);
    digitalWrite(17, LOW);
    digitalWrite(18, HIGH);
  }
}