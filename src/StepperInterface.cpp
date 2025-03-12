#include "StepperInterface.hh"
#include <math.h>

StepperInterface::StepperInterface() : moveSteps(0), currentStep(0), direction(0), stepsTarget(0) {

}
void StepperInterface::setStepsTarget(int newTarget){
  stepsTarget = newTarget;
}