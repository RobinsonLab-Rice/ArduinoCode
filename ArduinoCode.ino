
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
const int pumpEnable = 31;    //enable pin for syringe stepper
const int pumpStep = 26;      //step pin for syringe stepper
const int pumpDir = 30;       //dir pin for syringle stepper
const int calibrateX = 2;		//input 	used to calibrate, turns high when the arm is at (0,X)
const int calibrateY = 3;		//input 	used to calibrate, turns high when the arm is at (X,0)
const int yAxisEnable = 37;
const int yAxisStep = 34;
const int yAxisDir = 38;
const int xAxisEnable = 45;
const int xAxisStep = 42;
const int xAxisDir = 46;     //LOW is clockwise
const int xPAxisDir = 49;


/**
 * Maximum bounds that the arm is allowed to move over, in steps.
 */
const int X_BOUND = 9000;
const int Y_BOUND = 3720;

float XYSpeed = 6400.0;
float XYAccel = 1600.0;

/**
 * Servo that controls the nozzle on arm that lowers down.
 * TODO: encapsulate the SERVO_TYPE and timeForCompleteRotation in a constructor for wrapper servo class.
 */
//VarSpeedServo nozzleServo;
Servo nozzleServo;
const String SERVO_TYPE = "NORMAL";
boolean servoAttached = false;
unsigned long servoActivated = 0;
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

volatile boolean xRunning = false;
volatile boolean yRunning = false;
unsigned long XYActivated = 0;
boolean pumpRunning = false;
boolean xDirection = true;

boolean enableOFF = true;

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
  pinMode(xPAxisDir,OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready");
  calibrate();
  nozzleServo.detach();
}

/**
 * Continually called, just calls the motors' run() method continually, which moves them to a previously
 * specified position (or does nothing if the motor is already there). If the motors are done moving,
 * tell the computer software so that another command can be sent.
 *
 * TODO: put the runs in a timer with appropriate interval, and just put the arduino into a low power mode otherwise
 */
void loop()  {
  if (taskIsExecuting == 0) {
    xRunning = xMotor.run();
    yRunning = yMotor.run();
  if (xRunning == false && yRunning == false){
    Serial.println("Done");
    if (enableOFF)  {
      XYActivated = millis();
      //digitalWrite(xAxisEnable, HIGH);
      //digitalWrite(yAxisEnable, HIGH);
    }
    //digitalWrite(pumpEnable, HIGH);
    currentPosition[0] = xMotor.currentPosition();
    currentPosition[1] = yMotor.currentPosition();
    taskIsExecuting = -1;
  }}else if (XYActivated > 0)  {
    unsigned long temp = millis();
    if (temp - XYActivated > 1000 || XYActivated > temp)  {
      digitalWrite(xAxisEnable, HIGH);
      digitalWrite(yAxisEnable, HIGH);
      XYActivated = 0;
    }
  }
  if (servoAttached)  {
    unsigned long temp = millis();
    if (temp - servoActivated > 3000 || servoActivated > temp)  {
      nozzleServo.detach();
      servoAttached = false;
    }
  }
  /*
  if (taskIsExecuting == 1)  {
    pumpRunning = pumpMotor.run();
  if (pumpRunning == false){
    digitalWrite(pumpEnable, HIGH);
    delay(3000);
    Serial.println("Done");
    taskIsExecuting = -1;
  }}
  */

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
