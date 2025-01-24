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

//Colour Sensor Pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// L9958 slave select pins for SPI
#define SS_M4 14
#define SS_M3 13
#define SS_M2 12
#define SS_M1 11
// L9958 DIRection pins
#define DIR_M1 2
#define DIR_M2 3
#define DIR_M3 4
#define DIR_M4 7
// L9958 PWM pins
#define PWM_M1 9
#define PWM_M2 10  // Timer1
#define PWM_M3 5
#define PWM_M4 6  // Timer0


MPU6050 mpu6050(Wire);

// L9958 Enable for all 4 motors
#define ENABLE_MOTORS 8

int pwm1, pwm2, pwm3, pwm4;
boolean dir1, dir2, dir3, dir4;

//adjustment to account for motor discrepencies
float adj = 0.205;

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

//colour sensor variables
int redIntensity = 0;
int greenIntensity = 0;
int blueIntensity = 0;

//Running Median Setup
RunningMedian backDist = RunningMedian(11);
RunningMedian leftDist = RunningMedian(11);
RunningMedian rightDist = RunningMedian(11);
RunningMedian frontDist = RunningMedian(11);

void setup() {
  unsigned int configWord;

  // put your setup code here, to run once:
  pinMode(SS_M1, OUTPUT);
  digitalWrite(SS_M1, LOW);  // HIGH = not selected
  pinMode(SS_M2, OUTPUT);
  digitalWrite(SS_M2, LOW);
  pinMode(SS_M4, OUTPUT);
  digitalWrite(SS_M4, LOW);

  // L9958 DIRection pins
  pinMode(DIR_M1, OUTPUT);
  pinMode(DIR_M2, OUTPUT);
  pinMode(DIR_M4, OUTPUT);

  // L9958 PWM pins
  pinMode(PWM_M1, OUTPUT);
  digitalWrite(PWM_M1, LOW);
  pinMode(PWM_M2, OUTPUT);
  digitalWrite(PWM_M2, LOW);  // Timer1
  pinMode(PWM_M4, OUTPUT);
  digitalWrite(PWM_M4, LOW);  // Timer0

  // L9958 Enable for all 4 motors
  pinMode(ENABLE_MOTORS, OUTPUT);
  digitalWrite(ENABLE_MOTORS, HIGH);  // HIGH = disabled

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
  digitalWrite(SS_M1, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M1, HIGH);
  // Motor 2
  digitalWrite(SS_M2, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M2, HIGH);

  // Motor 4
  digitalWrite(SS_M4, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M4, HIGH);

  //Set initial actuator settings to pull at 0 speed for safety
  dir1 = 0;
  dir2 = 0;
  dir3 = 0;
  dir4 = 0;  // Set direction
  pwm1 = 0;
  pwm2 = 0;
  pwm3 = 0;
  pwm4 = 0;  // Set speed (0-255)

  digitalWrite(ENABLE_MOTORS, LOW);  // LOW = enabled

  delay(3000);

  //gyro, accel, disp set up
  //Serial.begin(19200);
  Wire.begin();                     
  Wire.beginTransmission(MPU);       
  Wire.write(0x6B);                  
  Wire.write(0x00);                  
  Wire.endTransmission(true);

  //Colour sensor pin setup
  //Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH); //freq scaling of 20 %
  digitalWrite(S1,LOW);

}  // End setup

void loop() {
  // forwards(55);
  // delay(1000);
  // backwards(55);
  // delay(1000);
  // right(55);
  // delay(1000);
  // left(55);
  // delay(1000);
  forwardRight(55);
}  //end void loop


void forwards(int speed) {
  dir1 = 1;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir4 = 0;
  pwm4 = speed;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);

  dir2 = 1;
  pwm2 = speed;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir3 = 0;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);
}

void backwards(int speed) {
  dir2 = 0;
  pwm2 = speed;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir3 = 1;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir1 = 0;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir4 = 1;
  pwm4 = speed;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void right(int speed) {
  dir2 = 1;
  pwm2 = speed;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir3 = 1;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir1 = 0;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir4 = 0;
  pwm4 = speed;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void left(int speed) {
  dir2 = 0;
  pwm2 = speed;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir3 = 0;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir1 = 1;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir4 = 1;
  pwm4 = speed;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void forwardRight(int speed) {
  dir1 = 0;
  pwm1 = 0;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir3 = 0;
  pwm3 = 0;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir2 = 1;
  pwm2 = speed - adj;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir4 = 0;
  pwm4 = speed - adj;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void backwardsRight(int speed) {
  dir1 = 0;
  pwm1 = 0;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir3 = 0;
  pwm3 = 0;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir2 = 0;
  pwm2 = speed - adj;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir4 = 1;
  pwm4 = speed - adj;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void forwardLeft(int speed) {
  dir1 = 1;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir3 = 0;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir2 = 0;
  pwm2 = 0;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir4 = 0;
  pwm4 = 0;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}

void backwardsLeft(int speed) {
  dir1 = 0;
  pwm1 = speed - adj;
  digitalWrite(DIR_M1, dir1);
  analogWrite(PWM_M1, pwm1);

  dir3 = 1;
  pwm3 = speed - adj;
  digitalWrite(DIR_M3, dir3);
  analogWrite(PWM_M3, pwm3);

  dir2 = 0;
  pwm2 = 0;
  digitalWrite(DIR_M2, dir2);
  analogWrite(PWM_M2, pwm2);

  dir4 = 0;
  pwm4 = 0;
  digitalWrite(DIR_M4, dir4);
  analogWrite(PWM_M4, pwm4);
}


void getAccVal(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); //Acc data register address: 0x3B
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  // divide the raw values by 16384 to get a range of +/- 2
  accX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  accY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  accZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
}

void getGyroVal(){
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

void calcDistTravelled(){
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;
  getAccVal();
  dispX = accX * elapsedTime * elapsedTime;
  dispY = accY * elapsedTime * elapsedTime;
}

int leftDist(){
   digitalWrite(leftTrig, LOW);
   delayMicroseconds(2);
   digitalWrite(leftTrig, HIGH);
   delayMicroseconds(10);
   digitalWrite(leftTrig, LOW);
   ultrasonicSignalDuration = pulseIn(leftEcho, HIGH,3000);
   leftDist.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}
int rightDist(){
   digitalWrite(rightTrig, LOW);
   delayMicroseconds(2);
   digitalWrite(rightTrig, HIGH);
   delayMicroseconds(10);
   digitalWrite(rightTrig, LOW);
   ultrasonicSignalDuration = pulseIn(rightEcho, HIGH,3000);
   leftDist.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}
int frontDist(){
   digitalWrite(frontTrig, LOW);
   delayMicroseconds(2);
   digitalWrite(frontTrig, HIGH);
   delayMicroseconds(10);
   digitalWrite(frontTrig, LOW);
   ultrasonicSignalDuration = pulseIn(frontEcho, HIGH,3000);
   leftDist.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}
int backDist(){
   digitalWrite(backTrig, LOW);
   delayMicroseconds(2);
   digitalWrite(backTrig, HIGH);
   delayMicroseconds(10);
   digitalWrite(backTrig, LOW);
   ultrasonicSignalDuration = pulseIn(backEcho, HIGH,3000);
   leftDist.add(microsecondsToCentimeters(ultrasonicSignalDuration));
}

void colourDetect(){
  //Red
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  redIntensity = map(pulseIn(sensorOut, LOW), XX, XX, 255,0);
  // Serial.print("R = ");
  // Serial.print(redColor);

  //Green
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  greenKIntensity = map(pulseIn(sensorOut, LOW), XX, XX, 255, 0);
  // Serial.print(" G = ");
  // Serial.print(greenColor);

  //Blue
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  blueIntensity = map(pulseIn(sensorOut, LOW), XX, XX, 255, 0);
  // Serial.print(" B = ");
  // Serial.print(blueColor);
}