#include <Arduino.h>

void setup() {
  pinMode(2,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  static int i = 0;
  
  Serial.println(i++);
  if (digitalRead(2) == HIGH)
  {
    digitalWrite(2, LOW);
  }
  else if (digitalRead(2) == LOW)
  {
    digitalWrite(2, HIGH);
  }
  
  delay(1000);
}