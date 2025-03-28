//#define FLIGHT_MODE //One stop shop, read sensors, log to SD, and buzzes.

//data source selector - use simulink file or default to onboard sensors
//#define SIMULINK_TESTING

//data destination selector - serial print or default to SD card
//#define WAIT_FOR_SERIAL //Note that this means the program won't run until you open a serial connection to the board.

//buzzer selector or default to blessed silence. deprecated on 3.x PCBs.
//#define BEEP


#include <Arduino.h>
#include "RocketRTOS.hh"
#include "Control.hh"
#include "QuickSilver.hh"
#include "Filter.hh"
#include "StateMachine.hh"
//#include <climits>
#include "RealServo.hh"

#ifdef FLIGHT_MODE
  #include "ExternalSensors.hh"
  #include "SDLogger.hh"
  #define BUZZ_PIN 6
#else
  #ifdef SIMULINK_TESTING
    #include "SimulinkData.hh" 
    SimulinkFile simIn;
  #else
    #include "ExternalSensors.hh"
    ExternalSensors sensors;
  #endif
  #ifdef WAIT_FOR_SERIAL
    #include "SDSpoofer.hh"
    SDSpoofer sd;
  #else
    #include "SDLogger.hh"
    SDLogger sd;
  #endif
  #ifdef BEEP
    #define BUZZ_PIN 6
  #else
    #define BUZZ_PIN 5
    #endif
#endif


#define BUZZ_TIME 125000 //0.125 sec
#define PAUSE_SHORT 500000 //0.5 sec
#define PAUSE_LONG 5000000 //5.0 sec
// #define PAUSE_LONG 2000000 //2.0 sec


#define LAUNCH_THRESHOLD_A_M_S2 10
#define LAUNCH_THRESHOLD_H_M 20
#define MIN_LOOPS_IN_STATE 3
#define MIN_POINTS_TO_LEAVE 3

#define G_TO_M_S2 9.8f

StateMachine state;
RealServo servo_obj;


inline void prvReadSensors();
inline void prvIntegrateAccel();
inline void prvDoControl();


static unsigned long microsNow=0;
static unsigned long microsOld = 0;
static unsigned long deltaMicros = 1;
static unsigned long testStartMicros = 0;
static unsigned long burnoutMicros = 0;
static unsigned long buzzMicros = 0;
static unsigned long altimeterMicros = 0;
static float burnoutTime=0; //time since burnout
float simTime = 0;
float newAcc=0;
float vel=0;
float h_raw=0;
float oldH=0;
float ang=0;
float h_groundLevel=-1.0f;
float intA=0;
float diffH=-1.0f;
float h_filtered=-1.0f;

float integralOfAccel = 0;

int h_resetCounter=0;

float a_raw[3] = {0.0f, 0.0f, 0.0f};
float a_filtered[3] = {0.0f, 0.0f, 0.0f};
float g_raw[3] = {0.0f, 0.0f, 0.0f};
float g_filtered[3] = {0.0f, 0.0f, 0.0f};
float dt = 0.01;
float dt_h = 0.023; //baro runs at 50 Hz (x8 oversampling -> 22.5 ms (19.5 ms typ) read time + 0.5 ms standby setting) //from datasheet :)

#define BACK_ACC_LENGTH 1000
float backAcc[BACK_ACC_LENGTH] = {0};
float backDt[BACK_ACC_LENGTH] = {0};
bool backCalcDone = false;
int backCalcIndex = 0;

float desiredH = 0;
float predictedH = 0;


//pt1Filter acc_filter[3];
QuickSilver attitude_estimate;
pt1Filter gyroFilters[3];
pt1Filter accFilters[3];
pt1Filter hFilter;

void setup(){
#ifdef WAIT_FOR_SERIAL
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Serial Connected");
  Serial.println("Attaching servo...");
#endif
//  servo_obj.attach(3);

  pinMode(BUZZ_PIN, OUTPUT);

//if the rocket will try to calibrate itself, beep 3 times quickly
#ifndef STATIC_OFFSETS
  digitalWrite(BUZZ_PIN, 1);
  delay(100);
  digitalWrite(BUZZ_PIN, 0);
  delay(100);
  digitalWrite(BUZZ_PIN, 1);
  delay(100);
  digitalWrite(BUZZ_PIN, 0);
  delay(100);
  digitalWrite(BUZZ_PIN, 1);
  delay(100);
  digitalWrite(BUZZ_PIN, 0);
#endif

  // initialize the filteres
  for (int axis = 0; axis < 3; axis++) {
    gyroFilters[axis].init(1.0, dt); // dt fed in here should be the rate at which we read new acc data
    accFilters[axis].init(5.0, dt);
  }
  hFilter.init(1.0, dt_h);

  attitude_estimate.initialize(0.005); // TODO tune beta to a reasonable value

#ifdef SIMULINK_TESTING
  Serial.println("Reading from simulation file");
  simIn.startupTasks("TEST2.csv");
  //simIn.printData();
#endif

#ifndef SIMULINK_TESTING
  sensors.startupTasks();
  sensors.readAltitude(h_groundLevel);
#endif

  // while(1){
  //   delay(1);
  //   sensors.readAcceleration(a_raw[0], a_raw[1], a_raw[2]);
  //   sensors.readGyroscope(g_raw[0], g_raw[1], g_raw[2]);
    
  //   Serial.print("GX:");
  //   Serial.print(g_raw[0]);
  //   Serial.print(", GY:");
  //   Serial.print(g_raw[1]);
  //   Serial.print(", GZ:");  
  //   Serial.print(g_raw[2]);
  //   Serial.print(", AX:");
  //   Serial.print(a_raw[0]);
  //   Serial.print(", AY:");
  //   Serial.print(a_raw[1]);
  //   Serial.print(", AZ:");
  //   Serial.println(a_raw[2]);

  // }

#ifndef STATIC_OFFSETS
  while(1){
    digitalWrite(BUZZ_PIN, 1);
    delay(100);
    digitalWrite(BUZZ_PIN, 0);
    delay(100);
    digitalWrite(BUZZ_PIN, 1);
    delay(100);
    digitalWrite(BUZZ_PIN, 0);
    delay(100);
    digitalWrite(BUZZ_PIN, 1);
    delay(100);
    digitalWrite(BUZZ_PIN, 0);
    delay(1000);
  }
#endif

  digitalWrite(BUZZ_PIN, 1);
  delay(1000);
  digitalWrite(BUZZ_PIN, 0);
  Serial.println("Flaps out");
//  servo_obj.setAngle(0.39);
//  servo_obj.write(2.39);
//  currentAngle=0.39;
  servo_obj.setAngle(degrees(2.39));
  delay(2000);
  //servo_obj.setAngle(0);
  Serial.println("Flaps in");
//  servo_obj.write(0);
  servo_obj.setAngle(0);
  delay(500);

  digitalWrite(BUZZ_PIN, 1);
  delay(1000);
  digitalWrite(BUZZ_PIN, 0);

#ifndef SIMULINK_TESTING
  sd.openFile("Acc, Vel, h_raw, h_filtered, h_ground, Ang, simT, burnoutT, State, DesiredH, PredictedH, intA, diffH, IntegralOfA, dt, 1/dt, dt_h, 1/dt_h, a_raw[0], a_raw[1], a_raw[2], a_filtered[0], a_filtered[1], a_filtered[2], g_raw[0], g_raw[1], g_raw[2], g_filtered[0], g_filtered[1], g_filtered[2], grav[0], grav[1], grav[2]");
#else
  sd.openFile("t(s), state, ang, desiredApogee, predictedApogee, burnoutTime, burnoutMicros, testStartMicros, micros");
  //sd.openFile("t, state, stepsTarget, currentStep, direction");
#endif

  testStartMicros = micros();
  startRocketRTOS();
  servo_obj.setAngle(0); // Initialize servo to 0 degrees
//  servo_obj.write(0);
}
void determineState(){
  state.updateState(h_filtered, vel, newAcc);
}


void sensorAndControl_PRE(){
  prvReadSensors(); 
  if(++h_resetCounter > 100000) //every 100 seconds
  {
    h_groundLevel += h_filtered; //when in pre-flight, update the ground level every nth pass
    h_resetCounter=0;
  }
}
void sensorAndControl_LAUNCH(){
  burnoutMicros = micros(); //TODO: what to do if overflow during this? Overflow occurs just over an hour after power-on
  prvReadSensors();
  prvIntegrateAccel();
}
void sensorAndControl_FULL(){
  prvReadSensors();
  prvIntegrateAccel();
  prvDoControl();
}

void logging_RUN(){
#ifndef SIMULINK_TESTING
  String log = String(newAcc) + String(", ") + String(vel) + String(", ") + String(h_raw) + String(", ") + String(h_filtered) + String(", ") + String(h_groundLevel) + String(", ") 
            + String(ang) + String(", ") + String(simTime) + String(", ") + String(burnoutTime) + String(", ") + String(rocketState) + String(", ") 
            + String(desiredH) + String(", ") + String(predictedH) + String(", ") + String(intA) + String(", ") + String(diffH) + String(", ") + String(integralOfAccel) + String(", ")
            + String(dt) + String(", ") + String((1.0f/dt)) + String(", ")  + String(dt_h) + String(", ") + String(1.0f/dt_h) + String(", ")
            + String(a_raw[0]) + String(", ") + String(a_raw[1]) + String(", ") + String(a_raw[2]) + String(", ") 
            + String(a_filtered[0]) + String(", ") + String(a_filtered[1]) + String(", ") + String(a_filtered[2]) + String(", ") 
            + String(g_raw[0]) + String(", ") + String(g_raw[1]) + String(", ") + String(g_raw[2]) + String(", ") 
            + String(g_filtered[0]) + String(", ") + String(g_filtered[1]) + String(", ") + String(g_filtered[2]) + String(", ")
            + String(attitude_estimate.getGravityVector()[0]) + String(", ") + String(attitude_estimate.getGravityVector()[1]) + String(", ") + String(attitude_estimate.getGravityVector()[2]);
#else 
  String log = String(simTime) + String(", ") + String(rocketState) + String(", ") + String(ang) + String(", ") + String(desiredH) + String(", ") + String(predictedH) + String(", ") + String(burnoutTime) + String(", ") + String(burnoutMicros) + String(", ") + String(testStartMicros) + String(", ") + String(microsNow);
  // String log =String(simTime) + String(", ") + String(ang);
  //String log = String(simTime) + String(", ") + String(rocketState) + String(", ") + String(stepperVars.stepsTarget) + String(", ") + String(stepperVars.currentStep) + String(", ") + String(stepperVars.direction);
  Serial.println(log);
#endif
sd.writeLine(log);
}
void logging_CLOSE(){
  //Serial.println("log close");
  sd.closeFile();
}
void logging_IDLE(){
  //Serial.println("log idle");
}

void buzz_PRE(){
  unsigned long localDiff = micros() - buzzMicros;
  if(localDiff > (BUZZ_TIME+PAUSE_LONG)){
    digitalWriteFast(BUZZ_PIN, 1);
    buzzMicros = micros();
  } else if(localDiff > BUZZ_TIME){
    digitalWriteFast(BUZZ_PIN, 0);
  }
}
void buzz_POST(){
  unsigned long localDiff = micros() - buzzMicros;
  if(localDiff > (BUZZ_TIME+PAUSE_SHORT+BUZZ_TIME+PAUSE_LONG)){
    digitalWrite(BUZZ_PIN, 1);
    buzzMicros = micros();
  } else if(localDiff > (BUZZ_TIME+PAUSE_SHORT+BUZZ_TIME)){
    digitalWrite(BUZZ_PIN, 0);
  } else if(localDiff > (BUZZ_TIME+PAUSE_SHORT)){
    digitalWrite(BUZZ_PIN, 1);
  } else if(localDiff > BUZZ_TIME){
    digitalWrite(BUZZ_PIN, 0);
  }
}
void buzz_IDLE(){
  digitalWriteFast(BUZZ_PIN, 0);
}




inline void prvReadSensors(){
  microsNow = micros();
  simTime = ((float)( microsNow - testStartMicros)) / 1000000.0f;
  burnoutTime = ((float)( microsNow - burnoutMicros )) / 1000000.0f;

  //calculate dt but catch any overflow error
  if(microsNow > microsOld){
    deltaMicros = (microsNow - microsOld);
  } else {
    deltaMicros = 10000;
  }

  dt = ((float)deltaMicros) / 1000000.0f;

  microsOld = microsNow;

#ifndef SIMULINK_TESTING
  sensors.readAcceleration(a_raw[0], a_raw[1], a_raw[2]);
  sensors.readGyroscope(g_raw[0], g_raw[1], g_raw[2]);

  //Apply Filters
  for (int axis = 0; axis < 3; axis++) {
    a_filtered[axis] = accFilters[axis].apply(a_raw[axis]);
    g_filtered[axis] = gyroFilters[axis].apply(g_raw[axis]);
  }

  //Update attitude estimate and extract vertical acceleration
  attitude_estimate.update_estimate(a_filtered, g_filtered, dt, rocketState==ROCKET_PRE); //Only fuse acc if rocket is in pre-flight state //WARNING: ensure that a_raw is in G's and that g_raw is in rad/s, and that dt is in seconds
  newAcc = attitude_estimate.vertical_acceleration_from_acc(a_filtered);
  newAcc *= G_TO_M_S2;
#else
   newAcc = simIn.getInterpolatedAcceleration(simTime);

#endif


  //read h and calculate diffH when a new value of h is ready
  if( (micros() - altimeterMicros) > 23000){ //19.5 ms is the typical pressure read case. Worst case is 22.5 ms. Standby is 0.5 ms
#ifndef SIMULINK_TESTING
    sensors.readAltitude(h_raw);
    h_raw -= h_groundLevel;
#else
    h_raw = simIn.getInterpolatedAltitude(simTime);
#endif

    oldH = h_filtered;
    h_filtered = hFilter.apply(h_raw);
    dt_h = (micros() - altimeterMicros) / 1000000.0f;
    altimeterMicros = micros();
    diffH = (h_filtered - oldH) / dt_h;
  }

  //fill back-calculation list
  backAcc[backCalcIndex] = newAcc;
  backDt[backCalcIndex] = dt;
  backCalcIndex = ((backCalcIndex+1)<=BACK_ACC_LENGTH) ? (backCalcIndex+1) : 0; 
}
inline void prvIntegrateAccel(){
  if(!backCalcDone){
    for(int i=BACK_ACC_LENGTH-1; i>=0; i--){
      // vel += backAcc[i] * backDt[i];
      intA += backAcc[i] * backDt[i]; //This should fix the mega spike at start?
    }
    backCalcDone = true;
  } else {
    intA = newAcc * dt; // will drift, but accurate over short times
  }

  float fusion_gain = 0.99; // how much we trust accelerometer data
  integralOfAccel += intA;
  // diffH = (h_filtered - oldH) / dt; //moved to prvReadSensors
  vel = fusion_gain * (vel + intA) + (1.0 - fusion_gain) * diffH;

}
inline void prvDoControl(){
  desiredH = getDesired(burnoutTime);
  predictedH = predictAltitude(h_filtered,vel);
  ang = getControl(desiredH, predictedH, dt);
  servo_obj.setAngle(ang); // Set servo angle directly
//  servo_obj.write(ang);
  delay(200);
}




// Uncomment lines 3 and 4 if you want to switch from actuall launch to simulated launch on ground
// Update File name on line 136 in Nova 2 and line 80 in Simulink Data.ccp