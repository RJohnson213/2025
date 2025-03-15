/****************************************************************************
* ServoInterface.hh 
* Base class for talking to a real, interrupting, or imagined servo motor
*
****************************************************************************/
#ifndef SERVO_INTERFACE_HH
#define SERVO_INTERFACE_HH

class ServoInterface {
protected:
//  Servo servo;
  float currentAngle;

public:
  ServoInterface();
  virtual void setAngle(float angle) = 0;
};

#endif // SERVO_INTERFACE_HH
