#include <AFMotor.h> 
#include <Servo.h>    
#include <NewPing.h>

#define TRIG_PIN A4 
#define ECHO_PIN A5
#define MAX_DISTANCE_POSSIBLE 1000 
#define MAX_SPEED 100 
#define MOTORS_CALIBRATION_OFFSET 3
#define COLL_DIST 20 
#define TURN_DIST COLL_DIST+10 
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE_POSSIBLE);

AF_DCMotor leftMotor(4, MOTOR12_8KHZ); 
AF_DCMotor rightMotor(3, MOTOR12_8KHZ); 
Servo neckControllerServoMotor;

int pos = 0; 
int maxDist = 0;
int maxAngle = 0;
int maxRight = 0;
int maxLeft = 0;
int maxFront = 0;
int course = 0;
int curDist = 0;
String motorSet = "";
int speedSet = 0;
bool servoDirection = true; // True: moving right, False: moving left

void setup() {
  neckControllerServoMotor.attach(10);  
  neckControllerServoMotor.write(90); 
  delay(2000);
  checkPath(); 
  motorSet = "FORWARD"; 
  neckControllerServoMotor.write(90);
  moveForward();
}

void loop() {
  checkForward();  
  checkPath();
}

void checkPath() {
  int curLeft = 0;
  int curFront = 0;
  int curRight = 0;
  int curDist = 0;

  if (servoDirection) {
    pos += 18; // Move right by 18 degrees
    if (pos >= 180) {
      pos = 180; // Limit the angle
      servoDirection = false; // Change direction to left
    }
  } else {
    pos -= 18; // Move left by 18 degrees
    if (pos <= 0) {
      pos = 0; // Limit the angle
      servoDirection = true; // Change direction to right
    }
  }

  neckControllerServoMotor.write(pos); // Update servo position
  delay(90); // Delay for smooth movement
  
  curDist = readPing(); // Measure the distance
  
  // Obstacle handling logic
  if (curDist < COLL_DIST) {
    checkCourse(); // Collision detected
  } else if (curDist < TURN_DIST) {
    changePath(); // Potential turn
  }

  // Update max distances for decision-making
  if (curDist > maxDist) { maxAngle = pos; maxDist = curDist; }
  if (pos > 90 && curDist > curLeft) { curLeft = curDist; }
  if (pos == 90 && curDist > curFront) { curFront = curDist; }
  if (pos < 90 && curDist > curRight) { curRight = curDist; }

  maxLeft = curLeft;
  maxRight = curRight;
  maxFront = curFront;
}

void setCourse() {
  if (maxAngle < 90) { turnRight(); }
  if (maxAngle > 90) { turnLeft(); }
  maxLeft = 0;
  maxRight = 0;
  maxFront = 0;
  maxDist = 0;
}

void checkCourse() {
  moveBackward();
  delay(500);
  moveStop();
  setCourse();
}

void changePath() {
  if (pos < 90) { lookLeft(); } 
  if (pos > 90) { lookRight(); }
}

int readPing() {
  delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS / US_ROUNDTRIP_CM;
  return cm;
}

void checkForward() {
  if (motorSet == "FORWARD") {
    leftMotor.run(FORWARD);
    rightMotor.run(FORWARD);
  }
}

void checkBackward() {
  if (motorSet == "BACKWARD") {
    leftMotor.run(BACKWARD);
    rightMotor.run(BACKWARD);
  }
}

void moveStop() {
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}

void moveForward() {
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    leftMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
    rightMotor.setSpeed(speedSet);
    delay(5);
  }
}

void moveBackward() {
  motorSet = "BACKWARD";
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    leftMotor.setSpeed(speedSet + MOTORS_CALIBRATION_OFFSET);
    rightMotor.setSpeed(speedSet);
    delay(5);
  }
}

void turnRight() {
  motorSet = "RIGHT";
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  delay(400);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}

void turnLeft() {
  motorSet = "LEFT";
  leftMotor.run(BACKWARD);
  rightMotor.run(FORWARD);
  delay(400);
  motorSet = "FORWARD";
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
}

void lookRight() {
  rightMotor.run(BACKWARD);
  delay(400);
  rightMotor.run(FORWARD);
}

void lookLeft() {
  leftMotor.run(BACKWARD);
  delay(400);
  leftMotor.run(FORWARD);
}