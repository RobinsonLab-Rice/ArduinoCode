
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
#include <Servo.h> 
#include "AccelStepper.h"


/**
 * Pin constants.
 * Initializing constants in multiple files gets very hacky very fast, the compiler just slaps all the
 * ino files together, and if the main .ino (this one) doesn't come first, it screws up. So, instead
 * of resigning myself to a shitty naming scheme, I moved all pin constants here.
 */
const int nozzleServoPin = 2;   //PWM		Servo for Fluid Handler
const int valvePin = 31;
const int pumpEnable = 32;    //enable pin for syringe stepper
const int pumpStep = 33;      //step pin for syringe stepper
const int pumpDir = 34;       //dir pin for syringle stepper
const int calibrateX = 35;		//input 	used to calibrate, turns high when the arm is at (0,X)
const int calibrateY = 37;		//input 	used to calibrate, turns high when the arm is at (X,0)
const int yAxisStep = 39;
const int yAxisDir = 40;
const int yAxisEnable = 41;
const int xAxisStep = 45;
const int xAxisDir = 46;     //LOW is clockwise
const int xAxisEnable = 47;
const int PowerPin = 49;    //Power Rail On\Off pin
const int pumpIn = 22;			//pin for the pump dc motor, can move it
const int pumpOut = 23;  // Allows the pump to move

/**
 * Maximum bounds that the arm is allowed to move over, in steps.
 */
const int X_BOUND = 9000;
const int Y_BOUND = 3720;

/**
 * Servo that controls the nozzle on arm that lowers down.
 * TODO: encapsulate the SERVO_TYPE and timeForCompleteRotation in a constructor for wrapper servo class.
 */
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
AccelStepper xMotor(AccelStepper::DRIVER, xAxisStep, xAxisDir);
AccelStepper yMotor(AccelStepper::DRIVER, yAxisStep, yAxisDir);
AccelStepper pumpMotor(AccelStepper::DRIVER, pumpStep, pumpDir);

/**
 * Current position of the arm, only to be used as a backup in case the software pushes it out of bounds
 */
int currentPosition[2] = {0, 0};

/**
 * Boolean for whether a task is currently being executed. Turns true when a task is processed and starts,
 * turns false when it is completed.
 */
boolean taskIsExecuting = false;

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
  Serial.begin(9600);
  Serial.println("Ready");
  calibrate();
  Serial.println("Finished Calibration");
}

/**
 * Continually called, just calls the motors' run() method continually, which moves them to a previously
 * specified position (or does nothing if the motor is already there). If the motors are done moving,
 * tell the computer software so that another command can be sent.
 *
 * TODO: put the runs in a timer with appropriate interval, and just put the arduino into a low power mode otherwise
 */
void loop()  {
	if ((xMotor.distanceToGo() == 0) && (yMotor.distanceToGo() == 0) && (pumpMotor.distanceToGo() == 0) && (taskIsExecuting == true)){
		//send back done so that we can have another task sent.
    Serial.println("Done");
    digitalWrite(xAxisEnable, HIGH);
    digitalWrite(yAxisEnable, HIGH);
    digitalWrite(pumpEnable, HIGH);
    taskIsExecuting = false;
  }
	xMotor.run();
	yMotor.run();
  pumpMotor.run();
}