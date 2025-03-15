#include "SimulinkServo.hh"
#include <Arduino.h>

SimulinkServo::SimulinkServo() {
  currentAngle = 0;
}

void SimulinkServo::setAngle(float angle) {
  currentAngle = angle;
  Serial.print("Set Angle: ");
  Serial.println(currentAngle);
}
