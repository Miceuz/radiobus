#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED_WAN, OUTPUT);
}

void loop() {
  digitalWrite(PIN_LED_WAN, HIGH);
  delay(300);
  digitalWrite(PIN_LED_WAN, LOW);
  delay(300);
  Serial.println("a\n");
}