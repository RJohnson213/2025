/****************************************************************************
* StepperInterface.hh 
* Base class for talking to a real, interrupting, or imagined stepper motor
*
****************************************************************************/
#ifndef STEPPER_INTERFACE_HH
#define STEPPER_INTERFACE_HH

class StepperInterface {
public:
  StepperInterface();
  //virtual void stepForTime(unsigned long maxTime, unsigned long startingTime)=0;
  virtual void stepOnce(void)=0;
  void setStepsTarget(int newTarget);
protected:
  int moveSteps;
  int currentStep;
  int direction;
  int stepsTarget;
};

#endif