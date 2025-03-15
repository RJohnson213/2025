#include "RealServo.hh"
#include <Arduino.h>
#include <Servo.h>

// Remove the definition of the Servo object
//Servo servo;

RealServo::RealServo() {
  // Use the Servo object from the base class
  
  //servo.attach(9); // Attach to pin 9
  //currentAngle = 0;
}

void RealServo::setAngle(float angle) {
  //currentAngle = angle;
  //servo.write(currentAngle);
}
