/*
23/6/2023
Base code Version 1 for Robocup Soccer Bot
Untested
*/

// include the SPI library:
#include <SPI.h>
//include IR Locator Library
#include <Wire.h>
//include MPU library
#include <MPU6050_tockn.h>
//include Running Median Library For Data Accuracy
#include <RunningMedian.h>


#define frontColourSensorS0Pin 27
#define frontColourSensorS1Pin 25
#define frontColourSensorS2Pin 23
#define frontColourSensorS3Pin 24
#define frontColourSensorOutPin 22

#define backColourSensorS0Pin 34
#define backColourSensorS1Pin 33
#define backColourSensorS2Pin 31
#define backColourSensorS3Pin 32
#define backColourSensorOutPin 30

#define leftColourSensorS0Pin 49
#define leftColourSensorS1Pin 50
#define leftColourSensorS2Pin 47
#define leftColourSensorS3Pin 48
#define leftColourSensorOutPin 46

#define rightColourSensorS0Pin 42
#define rightColourSensorS1Pin 41
#define rightColourSensorS2Pin 39
#define rightColourSensorS3Pin 40
#define rightColourSensorOutPin 838

#define motorSSPinM4 14
#define motorSSPinM3 13
#define motorSSPinM2 12
#define motorSSPinM1 11

#define motorDirectionPinM1 2
#define motorDirectionPinM2 3
#define motorDirectionPinM3 4
#define motorDirectionPinM4 7

#define motorPWMPinM1 9
#define motorPWMPinM2 10  // Timer1
#define motorPWMPinM3 5
#define motorPWMPinM4 6  // Timer0


MPU6050 mpu6050(Wire);


#define ENABLE_MOTORS_PIN 8

int pwm1, pwm2, pwm3, pwm4;
boolean dir1, dir2, dir3, dir4;

float motorAdjustmentValue = 0.205;

//declaration of gyro, accel, disp variables
const int MPU = 0x68; // MPU6050 I2C address
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime = 0;
int c = 0;
float dispX, dispY;


RunningMedian backDistance = RunningMedian(11);
RunningMedian leftDistance = RunningMedian(11);
RunningMedian rightDistance = RunningMedian(11);
RunningMedian frontDistance = RunningMedian(11);


RunningMedian colourSensorMedianValue = RunningMedian(11);


bool isColourWhiteRedAmount;
bool isColourWhiteBlueAmount;
bool isColourWhiteGreenAmount;


const int eyeAddress = 0x0E; //Address for IR Locator 360
RunningMedian irBallRunningMedian = RunningMedian(255);
int irBallValue; //This should be set as the value in fron of the robot

int SR04RightTrigPin = 28;
int SR04RightEchoPin = 29;
int SR04BackTrigPin = 37;
int SR04BackEchoPin = 36;
int SR04LeftTrigPin = 45;
int SR04LeftEchoPin = 44;

long ultrasonicSignalDuration;


bool isInAttackMode = true;
bool isSwitchingModes = false;

void setup() {
  unsigned int configWord;


  pinMode(motorSSPinM1, OUTPUT);
  digitalWrite(motorSSPinM1, LOW);  // HIGH = not selected
  pinMode(motorSSPinM2, OUTPUT);
  digitalWrite(motorSSPinM2, LOW);
  pinMode(motorSSPinM4, OUTPUT);
  digitalWrite(motorSSPinM4, LOW);

  // L9958 direction pins
  pinMode(motorDirectionPinM1, OUTPUT);
  pinMode(motorDirectionPinM2, OUTPUT);
  pinMode(motorDirectionPinM4, OUTPUT);

  // L9958 PWM pins
  pinMode(motorPWMPinM1, OUTPUT);
  digitalWrite(motorPWMPinM1, LOW);
  pinMode(motorPWMPinM2, OUTPUT);
  digitalWrite(motorPWMPinM2, LOW);  // Timer1
  pinMode(motorPWMPinM4, OUTPUT);
  digitalWrite(motorPWMPinM4, LOW);  // Timer0

  // L9958 Enable for all 4 motors
  pinMode(ENABLE_MOTORS_PIN, OUTPUT);
  digitalWrite(ENABLE_MOTORS_PIN, HIGH);  // HIGH = disabled

  /******* Set up L9958 chips *********
  ' L9958 Config Register
  ' Bit
  '0 - RES
  '1 - DR - reset
  '2 - CL_1 - curr limit
  '3 - CL_2 - curr_limit
  '4 - RES
  '5 - RES
  '6 - RES
  '7 - RES
  '8 - VSR - voltage slew rate (1 enables slew limit, 0 disables)
  '9 - ISR - current slew rate (1 enables slew limit, 0 disables)
  '10 - ISR_DIS - current slew disable
  '11 - OL_ON - open load enable
  '12 - RES
  '13 - RES
  '14 - 0 - always zero
  '15 - 0 - always zero
  */
  // set to max current limit and disable ISR slew limiting
  configWord = 0b0000010000001100;

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE1);  // clock pol = low, phase = high

  // Motor 1
  digitalWrite(motorSSPinM1, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(motorSSPinM1, HIGH);
  // Motor 2
  digitalWrite(motorSSPinM2, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(motorSSPinM2, HIGH);

  // Motor 4
  digitalWrite(motorSSPinM4, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(motorSSPinM4, HIGH);

  //Set initial actuator settings to pull at 0 speed for safety
  dir1 = 0;
  dir2 = 0;
  dir3 = 0;
  dir4 = 0;  // Set direction
  pwm1 = 0;
  pwm2 = 0;
  pwm3 = 0;
  pwm4 = 0;  // Set speed (0-255)

  digitalWrite(ENABLE_MOTORS_PIN, LOW);  // LOW = enabled

  delay(3000);

  //gyro, accel, disp set up
  //Serial.begin(19200);
  Wire.begin();                     
  Wire.beginTransmission(MPU);       
  Wire.write(0x6B);                  
  Wire.write(0x00);                  
  Wire.endTransmission(true);

  //Colour sensor pin setup
  Serial.begin(9600);
  pinMode(frontColourSensorS0Pin, OUTPUT);
  pinMode(frontColourSensorS1Pin, OUTPUT);
  pinMode(frontColourSensorS2Pin, OUTPUT);
  pinMode(frontColourSensorS3Pin, OUTPUT);
  pinMode(frontColourSensorOutPin, INPUT);
  digitalWrite(frontColourSensorS0Pin,HIGH); //freq scaling of 20 %
  digitalWrite(frontColourSensorS1Pin,LOW);

  pinMode(backColourSensorS0Pin, OUTPUT);
  pinMode(backColourSensorS1Pin, OUTPUT);
  pinMode(backColourSensorS2Pin, OUTPUT);
  pinMode(backColourSensorS3Pin, OUTPUT);
  pinMode(backColourSensorOutPin, INPUT);
  digitalWrite(backColourSensorS0Pin,HIGH); //freq scaling of 20 %
  digitalWrite(backColourSensorS1Pin,LOW);

  pinMode(leftColourSensorS0Pin, OUTPUT);
  pinMode(leftColourSensorS1Pin, OUTPUT);
  pinMode(leftColourSensorS2Pin, OUTPUT);
  pinMode(leftColourSensorS3Pin, OUTPUT);
  pinMode(leftColourSensorOutPin, INPUT);
  digitalWrite(leftColourSensorS0Pin,HIGH); //freq scaling of 20 %
  digitalWrite(leftColourSensorS1Pin,LOW);
  
  pinMode(rightColourSensorS0Pin, OUTPUT);
  pinMode(rightColourSensorS1Pin, OUTPUT);
  pinMode(rightColourSensorS2Pin, OUTPUT);
  pinMode(rightColourSensorS3Pin, OUTPUT);
  pinMode(rightColourSensorOutPin, INPUT);
  digitalWrite(rightColourSensorS0Pin,HIGH); //freq scaling of 20 %
  digitalWrite(rightColourSensorS1Pin,LOW);
}

void loop() {
  findRightDistance();

  irBallValue = irBallRunningMedian.getMedian();

  if(!isInAttackMode && isSwitchingModes) {
    defend(irBallValue);
  }
  else if(!isInAttackMode) {
    enterDefendMode(irBallValue);
    defend(irBallValue);
  }
  else {
    attack(irBallValue);
  }

  findBackDistance();

  if()

  
} 

void enterDefendMode(int directionOfBall) {

}

void attack(int directionOfBall) {

}

void defend(int directionOfBall) {

}

void forward(int speed) {
  dir1 = 1;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir4 = 0;
  pwm4 = speed;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);

  dir2 = 1;
  pwm2 = speed;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir3 = 0;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);
}

void backward(int speed) {
  dir2 = 0;
  pwm2 = speed;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir3 = 1;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir1 = 0;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir4 = 1;
  pwm4 = speed;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void left(int speed) {
  dir2 = 0;
  pwm2 = speed;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir3 = 0;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir1 = 1;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir4 = 1;
  pwm4 = speed;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void right(int speed) {
  dir2 = 1;
  pwm2 = speed;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir3 = 1;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir1 = 0;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir4 = 0;
  pwm4 = speed;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void forwardLeft(int speed) {
  dir1 = 1;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir3 = 0;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir2 = 0;
  pwm2 = 0;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir4 = 0;
  pwm4 = 0;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void forwardRight(int speed) {
  dir1 = 0;
  pwm1 = 0;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir3 = 0;
  pwm3 = 0;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir2 = 1;
  pwm2 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir4 = 0;
  pwm4 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void backwardLeft(int speed) {
  dir1 = 0;
  pwm1 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir3 = 1;
  pwm3 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir2 = 0;
  pwm2 = 0;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir4 = 0;
  pwm4 = 0;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void backwardRight(int speed) {
  dir1 = 0;
  pwm1 = 0;
  digitalWrite(motorDirectionPinM1, dir1);
  analogWrite(motorPWMPinM1, pwm1);

  dir3 = 0;
  pwm3 = 0;
  digitalWrite(motorDirectionPinM3, dir3);
  analogWrite(motorPWMPinM3, pwm3);

  dir2 = 0;
  pwm2 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM2, dir2);
  analogWrite(motorPWMPinM2, pwm2);

  dir4 = 1;
  pwm4 = speed - motorAdjustmentValue;
  digitalWrite(motorDirectionPinM4, dir4);
  analogWrite(motorPWMPinM4, pwm4);
}

void getAccelerometerValue() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); //Acc data register address: 0x3B
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  // divide the raw values by 16384 to get a range of +/- 2
  accX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  accY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  accZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
}

void getGyroscopeValue() {
  Wire.beginTransmission(MPU);
  Wire.write(0x43); //Gyro data register address: 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroZ = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet  
  // Correct the outputs with the calculated error values
  gyroZ = gyroZ + 0.79; // GyroErrorZ ~ (-0.8)
}

void calculateDistanceTravelled() {
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;
  getAccelerometerValue();
  dispX = accX * elapsedTime * elapsedTime;
  dispY = accY * elapsedTime * elapsedTime;
}

void findLeftDistance() {
   digitalWrite(SR04LeftTrigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(SR04LeftTrigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(SR04LeftTrigPin, LOW);
   ultrasonicSignalDuration = pulseIn(SR04LeftEchoPin, HIGH, 3000);
   leftDistance.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}

void findBackDistance() {
   digitalWrite(SR04BackTrigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(SR04BackTrigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(SR04BackTrigPin, LOW);
   ultrasonicSignalDuration = pulseIn(SR04BackEchoPin, HIGH, 3000);
   leftDistance.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}

void findRightDistance() {
   digitalWrite(SR04RightTrigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(SR04RightTrigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(SR04RightTrigPin, LOW);
   ultrasonicSignalDuration = pulseIn(SR04RightEchoPin, HIGH, 3000);
   rightDistance.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}


//this function will return a different number depending on what surface it is detecting
//0 (field/black line), 1 (white line)
void frontFieldColourDetect() {

  //Red
  digitalWrite(frontColourSensorS2Pin, LOW);
  digitalWrite(frontColourSensorS3Pin, LOW);
  // colourSensorRedIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Green
  digitalWrite(frontColourSensorS2Pin, HIGH);
  digitalWrite(frontColourSensorS3Pin, HIGH);
  // colourSensorGreenIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Blue
  digitalWrite(frontColourSensorS2Pin, LOW);
  digitalWrite(frontColourSensorS3Pin, HIGH);
  // colourSensorBlueIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //if white line detected
  //isColourWhiteRedAmount = (colourSensorRedIntensity < XX && colourSensorRedIntensity > XX);
  //isColourWhiteBlueAmount = (colourSensorBlueIntensity < XX && colourSensorBlueIntensity > XX);
  //isColourWhiteGreenAmount = (colourSensorRedIntesity < XX && colourSensorBlueIntensity > XX);
  // if(isColourWhiteRedAmount && isColourWhiteBlueAmount && isColourWhiteGreenAmount){
  //   return 1;
  // }

  // return 0;

}

void leftFieldColourDetect() {

  //Red
  digitalWrite(leftColourSensorS2Pin, LOW);
  digitalWrite(leftColourSensorS3Pin, LOW);
  // colourSensorRedIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Green
  digitalWrite(leftColourSensorS2Pin, HIGH);
  digitalWrite(leftColourSensorS3Pin, HIGH);
  // colourSensorGreenIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Blue
  digitalWrite(leftColourSensorS2Pin, LOW);
  digitalWrite(leftColourSensorS3Pin, HIGH);
  // colourSensorBlueIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);

  //if white line detected
  //isColourWhiteRedAmount = (colourSensorRedIntensity < XX && colourSensorRedIntensity > XX);
  //isColourWhiteBlueAmount = (colourSensorBlueIntensity < XX && colourSensorBlueIntensity > XX);
  //isColourWhiteGreenAmount = (colourSensorRedIntesity < XX && colourSensorBlueIntensity > XX);
  // if(isColourWhiteRedAmount && isColourWhiteBlueAmount && isColourWhiteGreenAmount){
  //   return 1;
  // }

  // return 0;

}

void backFieldColourDetect() {

  //Red
  digitalWrite(backColourSensorS2Pin, LOW);
  digitalWrite(backColourSensorS3Pin, LOW);
  // colourSensorRedIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Green
  digitalWrite(backColourSensorS2Pin, HIGH);
  digitalWrite(backColourSensorS3Pin, HIGH);
  // colourSensorGreenIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);

  //Blue
  digitalWrite(backColourSensorS2Pin, LOW);
  digitalWrite(backColourSensorS3Pin, HIGH);
  // colourSensorBlueIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //if white line detected
  //isColourWhiteRedAmount = (colourSensorRedIntensity < XX && colourSensorRedIntensity > XX);
  //isColourWhiteBlueAmount = (colourSensorBlueIntensity < XX && colourSensorBlueIntensity > XX);
  //isColourWhiteGreenAmount = (colourSensorRedIntesity < XX && colourSensorBlueIntensity > XX);
  // if(isColourWhiteRedAmount && isColourWhiteBlueAmount && isColourWhiteGreenAmount){
  //   return 1;
  // }

  // return 0;

}

void rightFieldColourDetect() {

  //Red
  digitalWrite(rightColourSensorS2Pin, LOW);
  digitalWrite(rightColourSensorS3Pin, LOW);
  // colourSensorRedIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Green
  digitalWrite(rightColourSensorS2Pin, HIGH);
  digitalWrite(rightColourSensorS3Pin, HIGH);
  // colourSensorGreenIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //Blue
  digitalWrite(rightColourSensorS2Pin, LOW);
  digitalWrite(rightColourSensorS3Pin, HIGH);
  // colourSensorBlueIntensity = map(pulseIn(colourSensorOutPin, LOW), XX, XX, 255, 0);


  //if white line detected
  //isColourWhiteRedAmount = (colourSensorRedIntensity < XX && colourSensorRedIntensity > XX);
  //isColourWhiteBlueAmount = (colourSensorBlueIntensity < XX && colourSensorBlueIntensity > XX);
  //isColourWhiteGreenAmount = (colourSensorRedIntesity < XX && colourSensorBlueIntensity > XX);
  // if(isColourWhiteRedAmount && isColourWhiteBlueAmount && isColourWhiteGreenAmount){
  //   return 1;
  // }

  // return 0;

}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

int irBallDetect() {
  Wire.beginTransmission(eyeAddress);
  Wire.write(0x04); //This is where the 12000 Hz ball signal is stored
  Wire.endTransmission();
  Wire.requestFrom(eyeAddress, 4); // Ask for 4 bytes
  while(Wire.available()) { // Get the 4 bytes
    irBallRunningMedian.add(Wire.read()); // receive a byte
  }
}

//returns 0 if robot should defend and 1 if robot should attack
bool communicateWithOtherRobot(float directionOfBall) {

}
