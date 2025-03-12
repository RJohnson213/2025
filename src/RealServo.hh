/******************************************************************************
* RealServo.hh
*
* Functions and constants to make our motor move to the correct location
* Derived from ServoStuff and renamed to clarify: works with actual motor
*
* 02/15/2024 - Created file
******************************************************************************/
#ifndef REAL_SERVO_HH
#define REAL_SERVO_HH

#include "ServoInterface.hh"

class RealServo : public ServoInterface {
public:
  RealServo();
  virtual void setAngle(float angle) override;
};

#endif //REAL_SERVO_HH
