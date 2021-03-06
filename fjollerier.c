#pragma config(Sensor, S1,     LightSensor,    sensorEV3_Color)
#pragma config(Sensor, S2,     PressureSensor, sensorEV3_Touch, modeEV3Bump)
#pragma config(Sensor, S3,     DistanceSensor, sensorEV3_Ultrasonic)
#pragma config(Sensor, S4,     GyroSensor,     sensorEV3_Gyro)
#pragma config(Motor,  motorA,          leftmotor,     tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor,  motorB,          rightmotor,    tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)
#pragma config(Motor,  motorC,          armmotor,      tmotorEV3_Medium, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define SPEED 60
#define LIGHTMARGIN 4
#define GYROMARGIN 2

/*--GLOBAL VARIABLES--*/
//Driving
int leftspeed, rightspeed, lcalibspeed, rcalibspeed;

//Light
int linepresent, calibratedLight, light, blackline;

//Distance
int distance, objectclose;

//Others
int armstate;
int lineCount;

/*--GLOBAL FUNCTIONS--*/
//Driving
void drive(int speeda, int speedb);
void stop();
void followLine();
void turn(int deg);
void wheelTurns(int turns);

//Light
void updateLight();
void init();

//Distance
void updateUltraSound();

//Others
void loop(); //Main program loop
void pickUpBottle(); //Function to pick up the bottle
void toggleArm(); //Open/close arm
void lineCounter(); //Count witch black line we're at
void blackLineFunction(int count); //switch/case. Run the case according to the black line we're at


/*--BEGIN PROGRAM--*/
task main()
{
	init(); //Initialize function
  loop(); //Start the loop
}

void init() {
    //Set initial values
		lineCount = 0;
		armstate = true;
		leftspeed = SPEED;
		rightspeed = SPEED-10;
		lcalibspeed = -50;
		rcalibspeed = 50;
		
    delay(500); //Wait some time before start
    
    calibratedLight = SensorValue(LightSensor); //Get the linecolor to follow
    
	  updateLight(); //Use function to update some initial values 
	  
    displayBigTextLine(2,"Calib-Light:%d",calibratedLight); //Write the calibrated light value on screen
}

void loop() {
	while(true) {
	
		updateLight(); //Reads the light sensor, updates "linepresent"
		updateUltraSound(); //updates "objectclose"
		
		if(linepresent)
			drive(leftspeed, rightspeed); //if the line is present: drive
		else
			followLine(); //correct the direction

		if(objectclose)
		  pickUpBottle(); //if object is close to ultrasound: pick up bottle

		if(blackline) {
			lineCounter();
			blackLineFunction(lineCount); //if black line is present: run the according function in blackLineFunction
		}
	}
}

void updateLight() {
	light = SensorValue(LightSensor); //get light sensor value
	linepresent = light < calibratedLight+LIGHTMARGIN && light > calibratedLight-LIGHTMARGIN; //check if line is present
	blackline = light > 90; //LOOKING FOR WHITE LINE //check if black line is present
  displayBigTextLine(4, "Light: %d", light); //write sensor value on screen
}

void updateUltraSound() {
  distance = SensorValue(DistanceSensor); //get distance in cm
  objectclose = distance<5; //check if there is an object within 5cm
  displayBigTextLine(6, "Distance: %dcm", distance); //write distance on screen
}

void followLine() {
	stop();
	drive(lcalibspeed,rcalibspeed); //adjusting direction
}

void lineCounter() {
	lineCount++; //add 1 to lineCount so we know what obstacle we're at
	displayBigTextLine(8, "Linecount:%d", lineCount); //write which blackline, we are at, on screen
}

//OBSTACLE FUNCTIONS

void pickUpBottle() {
    stop();
    turn(180); //make uturn
    while(SensorValue(PressureSensor) > 0) {
      drive(-60, -60); //back up until button is pressed
    }
    toggleArm(); //close arm grabbing bottle
}

void toggleArm() {
    if(armstate) {
      motor[armmotor] = -100; //if arm is open: close arm
      armstate = !armstate; //invert armstate from true to false
    }
    else {
      motor[armmotor] = 100; //if arm is closed: open arm
      armstate = !armstate; //invert armstate
    }

    delay(1000); //run the open/close-function for 1 sec
    motor[armmotor] = 0; //stop motor
}
//DRIVING FUNCTIONS

void drive(int speeda, int speedb) {
  motor[leftmotor] = speeda;
  motor[rightmotor] = speedb;
}

void stop() {
	motor[leftmotor] = 0;
	motor[rightmotor] = 0;
}

void turn(int deg) {
    int old = SensorValue[GyroSensor]; //get the current value of the gyroscope
    int new = old + deg;
    delay(500); 
    while(SensorValue[GyroSensor] < new+GYROMARGIN && SensorValue[GyroSensor] > new-GYROMARGIN)
    		deg<0 ? drive(20,-20) : drive(-20,20); //turn while gyro value is not +-2 new value
    stop();
}

void wheelTurns(int turns) {
	nMotorEncoder[leftmotor] = 0; //reset the motor encoder that counts wheelturns in degrees
	while(nMotorEncoder[leftmotor] < turns*360) //THIS NUMBER NEEDS CHANGE CUZ OF THE GEARING
		drive(SPEED, SPEED); //drive while motor encoder is less than the wheelturns, we want.
	stop();
}

//OBSTACLE FUNCTION BELOW
//VERY LONG
//WATCH OUT
//UGLY SHIT

//---------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------//

void blackLineFunction(int count) {
	switch(count) {
		case 0:
		case 1:
			wheelTurns(2);
			turn(90);
			while(!linepresent)
				drive(SPEED, SPEED);
			turn(-90);
			break;
		case 2:
			wheelTurns(2);
			turn(-90);
			while(!linepresent)
				drive(SPEED, SPEED);
			turn(90);
			break;
		case 3: //Maybe not neccessary, just do normal linefollow and it will turn left...
			wheelTurns(1);
			turn(90);
			wheelTurns(2);
			break;
		case 4: //Maybe just do linefollow left?
			wheelTurns(2);
			turn(-90);
			wheelTurns(2);
			break;
		case 5:
			delay(4000); //Continue for four secs
			rightspeed = SPEED-10; //This makes linefollow left
			leftspeed = SPEED;
			rcalibspeed = -50;
			lcalibspeed = 50;
			break;
		case 6:
			wheelTurns(4);
			turn(-90);
			wheelTurns(5);
			while(!linepresent)
				drive(SPEED, SPEED);
			turn(90);
			break;
		case 7: //circle with the bottles
			/*
			wheelTurns(2);
			turn(270);
			wheelTurns(1);
			break;
			*/
		case 8:
			turn(90);
			wheelTurns(3);
			turn(-90);
			wheelTurns(3);
			turn(-90);
			while(!linepresent)
				drive(SPEED, SPEED);
			turn(90);
			break;
		case 9:
			while(!objectclose)
				drive(SPEED, SPEED);
			turn(-45);
			wheelTurns(4);
			turn(90);
			wheelTurns(4);
			turn(-45);
			while(!linepresent)
				drive(SPEED, SPEED);
			break;
		case 10:
			turn(-90);
			wheelTurns(3);
			turn(90);
			wheelTurns(3);
			turn(90);
			while(!linepresent)
				drive(SPEED, SPEED);
			turn(-90);
			break;
		case 11:
			wheelTurns(20);
			while(true)
				drive(SPEED, -SPEED);
			break;
	}
}