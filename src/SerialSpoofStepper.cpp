#include "SerialSpoofStepper.hh"
#include <Arduino.h>
#include <math.h>

SerialSpoofStepper::SerialSpoofStepper(){
  direction = 0;
  stepsTarget = 0;
  currentStep = 0;
}

void SerialSpoofStepper::stepForTime(unsigned long maxTime, unsigned long startingTime){
    
}

void SerialSpoofStepper::stepOnce(void){
  int direction = -1;
  if ((currentStep-stepsTarget)== 0) {
    return;
  } else if((currentStep-stepsTarget) > 0 ){
    direction = 0;
  } else {
    direction = 1;
  }
  if (direction) {
    currentStep++;
  } else {
    currentStep--;
  }
  Serial.print("Step ");
  Serial.println(currentStep);
  delay(PULSE_PERIOD_MS);
}

void SerialSpoofStepper::setStepsTarget(int newTarget){
  Serial.print("New Goal: ");
  Serial.println(newTarget);
  stepsTarget = newTarget;
}
