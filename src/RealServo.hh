/******************************************************************************
* RealServo.hh
*
* Functions and constants to make our motor move to the correct location
* Derived from ServoStuff and renamed to clarify: works with actual motor
*
* 02/15/2024 - Created file
* 03/22/2025 - Moved to PWM-based servo implementation
******************************************************************************/
#ifndef REAL_SERVO_HH
#define REAL_SERVO_HH
// Teensy pin number for control signal
// Teensy 4.1 has hardware PWM on these pins.
//	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 22, 23, 24, 25, 28, 29, 33, 36, 37, 42, 43, 44, 45, 46, 47, 51, 54
#define CONTROL_PIN 3 
#define PWM_FREQ 4482 //Hz

//Servo specs from this
//https://www.amazon.com/ANNIMOS-Coreless-Steering-Stainless-Waterproof/dp/B0C69BXL6V?th=1 

//Pulse ranges, us
#define SHORT_PULSE 500 // 2000Hz
#define NEUTRAL_PULSE 1500 // 667 Hz?
#define LONG_PULSE 2500 // 400Hz
//Motion limits, degrees
#define MIN 0.0f
#define MAX 270.0f

#include "ServoInterface.hh"
#include <Arduino.h>

class RealServo : public ServoInterface {
public:
  RealServo();
  virtual void setAngle(float angle) override;
};

#endif //REAL_SERVO_HH
