/**
 * Initializes motors, setting their default speed, acceleration, and calibrate pins. Step and dir pins are
 * automatically set by the AccelStepper library.
 */
void initializeMotors(){	
	yMotor.setMaxSpeed(2400.0);
	yMotor.setAcceleration(1800.0);

	xMotor.setMaxSpeed(2400.0);
	xMotor.setAcceleration(1800.0);

	//set calibrate pins to be inputs, check when calibrate() is called
	pinMode(calibrateX, INPUT); 
	pinMode(calibrateY, INPUT);
}

/**
 * Initializes servos by setting their initial pin states and attaching servos to the pins.
 */
void initializeServos(){
	nozzleServo.attach(nozzleServoPin);
}