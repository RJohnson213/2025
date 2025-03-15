#include "SerialSpoofServo.hh"
#include <Arduino.h>

SerialSpoofServo::SerialSpoofServo() {
  currentAngle = 0;
}

void SerialSpoofServo::setAngle(float angle) {
  currentAngle = angle;
  Serial.print("Set Angle: ");
  Serial.println(currentAngle);
}
