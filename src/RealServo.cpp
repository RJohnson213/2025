#include "RealServo.hh"

#define pulseToDuty(pulse) (pulse)/(1000000.0f / 50.0f) 

RealServo::RealServo() {
  analogWriteFrequency(CONTROL_PIN,50); //it's a hobby servo.
  analogWrite(CONTROL_PIN,256*pulseToDuty(SHORT_PULSE));
  currentAngle = 0;
}

void RealServo::setAngle(float angle) {
  //floor/ceiling commanded angle
  if (angle < MIN)
    angle = MIN;
  if (angle > MAX)
    angle = MAX;
  
  currentAngle = angle;
  angle = map(angle, MIN, MAX, SHORT_PULSE, LONG_PULSE);
  analogWrite(CONTROL_PIN,256*pulseToDuty(angle));
}
