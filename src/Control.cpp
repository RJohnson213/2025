#include "Control.hh"
#include <Arduino.h>


float getDesired(float time) {
  return 167.0f * powf(M_E, -time) + 590.0f;
}


float predictAltitude(float height, float velocity){
  return height + velocity*velocity/2.0f/9.81f;
}

float integratorState = 0;
float getControl(float desired, float predicted, float dt){
  float err = desired - predicted;
  float control = CONTROL_P * err; 
  integratorState += CONTROL_I*err*dt;
  // control += integratorState; // I included this
  control += CONTROL_BIAS;
  // Serial.println(control);
  if (control > 140 * M_PI / 180) control = 140 * M_PI / 180;
  if (control < 0) control = 0;
  return control;
}