/******************************************************************************
* SpoofServo.hh
*
* Fakes the potentiometer behavior for easier simulator testing
* Uses same function names and footprints as ServoStuff.
* 
* 02/19/2024 - Created file1
******************************************************************************/
#ifndef SPOOF_SERVO_HH
#define SPOOF_SERVO_HH

#include "ServoInterface.hh"

class SpoofServo : public ServoInterface {
public:
  SpoofServo();
  virtual void setAngle(float angle) override;
};

#endif  //SPOOF_SERVO_HH
