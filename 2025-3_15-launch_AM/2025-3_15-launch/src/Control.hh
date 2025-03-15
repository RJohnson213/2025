/******************************************************************************
* Control.hh
*
* Contains all the control functions and constants
*
* 02/15/2024 - Created file
* 02/29/2024 - Updated with newest equations and values
******************************************************************************/


#ifndef CONTROL_HH
#define CONTROL_HH
#include <math.h>


float getDesired(float time);

float predictAltitude(float height, float velocity);

#define CONTROL_P       (-0.6f)//(-0.6f)
#define CONTROL_I       (-0.005f)
#define CONTROL_BIAS    (0.1f) //Yes this should be negative
#define UPPER_CONTROL_SATURATION (140 * M_PI / 180)
 
extern float integratorState;
float getControl(float desired, float predicted, float dt);



#endif //CONTROL_HH
