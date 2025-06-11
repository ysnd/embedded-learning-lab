#include <Arduino.h>
#define LED 2

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(LED, HIGH);
  Serial.println("ON");
  delay(1000);
  digitalWrite(LED, LOW);
  Serial.println("OFF");
  delay(1000);
}

