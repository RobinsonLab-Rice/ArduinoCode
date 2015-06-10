
/**
 * This is the main operating code for the Arduinos used in RobinsonLab at Rice University.  The Arduino
 * is controlled by serial commands, sent by a MATLAB or Java interface.  These commands are parsed, then
 * the appropriate movements are made on the hardware.
 */

/**
 * Included headers, make sure to download AccelStepper from:
 * http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
 * and install it in the libraries folder of the Arduino program file.
 */
//#include <VarSpeedServo.h>
#include <Servo.h>
#include "AccelStepper.h"

/**
 * Pin constants.
 * Initializing constants in multiple files gets very hacky very fast, the compiler just slaps all the
 * ino files together, and if the main .ino (this one) doesn't come first, it screws up. So, instead
 * of resigning myself to a shitty naming scheme, I moved all pin constants here.
 */
const int nozzleServoPin = 22;   //PWM		Servo for Fluid Handler
const int valvePin = 31;
const int pumpEnable = 32;    //enable pin for syringe stepper
const int pumpStep = 33;      //step pin for syringe stepper
const int pumpDir = 34;       //dir pin for syringle stepper
const int calibrateX = 35;		//input 	used to calibrate, turns high when the arm is at (0,X)
const int calibrateY = 37;		//input 	used to calibrate, turns high when the arm is at (X,0)
const int yAxisEnable = 36;
const int yAxisStep = 38;
const int yAxisDir = 39;
const int xAxisEnable = 40;
const int xAxisStep = 41;
const int xAxisDir = 42;     //LOW is clockwise


//pin assignments for RAMPS board
// const int nozzleServoPin = 4;   //PWM   Servo for Fluid Handler
// const int valvePin = 31;
// const int pumpEnable = 24;    //enable pin for syringe stepper
// const int pumpStep = 26;      //step pin for syringe stepper
// const int pumpDir = 28;       //dir pin for syringle stepper
// const int calibrateX = 35;    //input   used to calibrate, turns high when the arm is at (0,X)
// const int calibrateY = 37;    //input   used to calibrate, turns high when the arm is at (X,0)
// const String yAxisEnable = A2;
// const String yAxisStep = ;
// const int yAxisDir = ;
// const int xAxisEnable = ;
// const int xAxisStep = ;
// const int xAxisDir = ;     //LOW is clockwise

/**
 * Maximum bounds that the arm is allowed to move over, in steps.
 */
const int X_BOUND = 9000;
const int Y_BOUND = 3720;

/**
 * Servo that controls the nozzle on arm that lowers down.
 * TODO: encapsulate the SERVO_TYPE and timeForCompleteRotation in a constructor for wrapper servo class.
 */
//VarSpeedServo nozzleServo;
Servo nozzleServo;
const String SERVO_TYPE = "NORMAL";
/**
 * Amount of time (in mS) the servo takes for a 180 degree rotation, should be changed depending on end servo.
 */
const int timeForCompleteRotation = 500;

/**
 * Motors that control the x-y movement of the arm.
 * TODO: fill the 3rd and 4th parameter with appropriate values, I just guessed here.
 */
AccelStepper xMotor(1, xAxisStep, xAxisDir);
AccelStepper yMotor(1, yAxisStep, yAxisDir);
AccelStepper pumpMotor(1, pumpStep, pumpDir);

boolean xRunning = false;
boolean yRunning = false;
boolean pumpRunning = false;
boolean xDirection = true;

/**
 * Current position of the arm, only to be used as a backup in case the software pushes it out of bounds
 */
int currentPosition[2] = {0, 0};

/**
 * Boolean for whether a task is currently being executed. Turns true when a task is processed and starts,
 * turns false when it is completed.
 */
int taskIsExecuting = -1;

/**
 * We need a string to keep track of the serial input, because it could not all come in at once.
 */
String serialInput = "";

/**
 * First thing called in the program, calls functions to initialize pins and starts serial communication.
 */
void setup()  {
	initializeMotors();		//Set initial speed and acceleration values of all motors
	initializeServos();     //Gives all servos their appropriate pin
  pinMode(49,OUTPUT);
  Serial.begin(9600);
  Serial.println("Ready");
  calibrate();
}

/**
 * Continually called, just calls the motors' run() method continually, which moves them to a previously
 * specified position (or does nothing if the motor is already there). If the motors are done moving,
 * tell the computer software so that another command can be sent.
 *
 * TODO: put the runs in a timer with appropriate interval, and just put the arduino into a low power mode otherwise
 */
void loop()  {
  xRunning = xMotor.run();
  yRunning = yMotor.run();
  pumpRunning = pumpMotor.run();
  if (taskIsExecuting == 0) {
  if (xRunning == false && yRunning == false){
    Serial.println("Done");
    digitalWrite(xAxisEnable, HIGH);
    digitalWrite(yAxisEnable, HIGH);
    taskIsExecuting = -1;
  }}
  if (taskIsExecuting == 1)  {
  if (pumpRunning == false){
    digitalWrite(pumpEnable, HIGH);
    delay(3000);
    Serial.println("Done");
    taskIsExecuting = -1;
  }}

	// if ((xMotor.distanceToGo() == 0) && (yMotor.distanceToGo() == 0) && (pumpMotor.distanceToGo() == 0) && (taskIsExecuting == true)){
	// 	//send back done so that we can have another task sent.

 //    Serial.println("Done");
 //    digitalWrite(xAxisEnable, HIGH);
 //    digitalWrite(yAxisEnable, HIGH);
 //    digitalWrite(pumpEnable, HIGH);
 //    taskIsExecuting = false;
 //  }
  /*
  if (xMotor.distanceToGo() > 0)  {
    if (!xDirection)  {
      digitalWrite(49,HIGH);
      xDirection = true;
    }
  }  else  {
    if (xDirection)  {
      digitalWrite(49,LOW);
      xDirection = false;
    }
  } */
}
