#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 12;
const int camTriggerPin = 13;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(camTriggerPin, OUTPUT);
  // Initialize LCD if you are using it
  // lcd.begin(13, 2);
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;  // Calculate the distance in centimeters

  return distance;
}

void loop() {
  float distance = getDistance();
  if (distance <= 50) {
    digitalWrite(buzzerPin, HIGH);       // Activate the buzzer
    digitalWrite(camTriggerPin, HIGH);  // Trigger the ESP32CAM
  } else {
    digitalWrite(buzzerPin, LOW);        // Deactivate the buzzer
    digitalWrite(camTriggerPin, LOW);   // Turn off the ESP32CAM trigger
  }

  // Optional: Display the distance on an LCD
  // lcd.setCursor(0, 0);
  // lcd.print("Distance: ");
  // lcd.print(distance);
  // lcd.print(" cm");

  delay(100);
}
