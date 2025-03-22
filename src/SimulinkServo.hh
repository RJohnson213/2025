/****************************************************************************
* SimulinkServo.hh 
* Class for providing Simulink with feedback by simulating a servo
****************************************************************************/
#ifndef SIMULINK_SERVO_HH
#define SIMULINK_SERVO_HH

#include "ServoInterface.hh"

class SimulinkServo : public ServoInterface {
public:
  SimulinkServo();
  virtual void setAngle(float angle) override;
  float currentAngle=0.0f;
};

#endif //SIMULINK_SERVO_HH
