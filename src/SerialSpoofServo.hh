/******************************************************************************
 * SerialSpoofServo.hh
 * 
 * A Spoof Servo alternative that prints to serial rather than steps.
 * Useful for debugging in RocketRTOS
 * 
 * 02/24/24 - Created File
 * 
******************************************************************************/
#ifndef SERIAL_SPOOF_SERVO_HH
#define SERIAL_SPOOF_SERVO_HH

#include "ServoInterface.hh"

class SerialSpoofServo : public ServoInterface {
public:
  SerialSpoofServo();
  void setAngle(float angle) override;
};

#endif //SERIAL_SPOOF_SERVO_HH
