/**
 * Initializes motors, setting their default speed, acceleration, and calibrate pins. Step and dir pins are
 * automatically set by the AccelStepper library.
 */
void initializeMotors(){	
	pinMode(pumpEnable, OUTPUT);
	pinMode(pumpStep, OUTPUT);
	pinMode(pumpDir, OUTPUT);

	pinMode(xAxisEnable, OUTPUT);
	pinMode(xAxisStep, OUTPUT);
	pinMode(xAxisDir, OUTPUT);

	pinMode(yAxisEnable, OUTPUT);
	pinMode(yAxisStep, OUTPUT);
	pinMode(yAxisDir, OUTPUT);

	pinMode(calibrateX, INPUT);
	pinMode(calibrateY, INPUT);

	digitalWrite(xAxisEnable, HIGH);
	digitalWrite(yAxisEnable, HIGH);
	digitalWrite(pumpEnable, HIGH);

	yMotor.setMaxSpeed(2400.0);
	yMotor.setAcceleration(1800.0);

	xMotor.setMaxSpeed(2400.0);
	xMotor.setAcceleration(1800.0);

	//pumpMotor.setMaxSpeed(300);
	//pumpMotor.setAcceleration(200);
	pumpMotor.setMaxSpeed(1000);
	pumpMotor.setAcceleration(500);

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