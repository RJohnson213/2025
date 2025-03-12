#include "RealServo.hh"
#include <Arduino.h>
#include <Servo.h>

Servo servo;

RealServo::RealServo() {
  servo.attach(9); // Attach to pin 9
  currentAngle = 0;
}

void RealServo::setAngle(float angle) {
  currentAngle = angle;
  servo.write(currentAngle);
}
