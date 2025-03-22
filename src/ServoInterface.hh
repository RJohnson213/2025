/****************************************************************************
* ServoInterface.hh 
* Base class for talking to a real, interrupting, or imagined servo motor
* 3/22/2025 - dropped cpp file, abstract classes are headers
****************************************************************************/
#ifndef SERVO_INTERFACE_HH
#define SERVO_INTERFACE_HH

class ServoInterface {
public:
  ServoInterface(){}
  float currentAngle = 0.0f;
  virtual void setAngle(float angle) = 0;
};

#endif // SERVO_INTERFACE_HH
