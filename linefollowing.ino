#include <NewPing.h> 
#include <Servo.h>
#include <AFMotor.h>

// HC-SR04 Sensor
#define TRIGGER_PIN A2
#define ECHO_PIN A3
#define MAX_DISTANCE 50

// IR Sensors
#define irLeft A0
#define irRight A1

// Servo Motor
Servo servo;

// Sonar Sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Motors
AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

int distance = 0;
int leftDistance;
int rightDistance;
boolean object;

void setup() {
  Serial.begin(9600);
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);
  servo.attach(10);
  servo.write(90);

  motor1.setSpeed(100);
  motor2.setSpeed(100);
  motor3.setSpeed(100);
  motor4.setSpeed(100);
}

void loop() {
  if (digitalRead(irLeft) == 0 && digitalRead(irRight) == 0) {
    objectAvoid();
    moveForward();
  } 
  else if (digitalRead(irLeft) == 0 && digitalRead(irRight) == 1) {
    objectAvoid();
    Serial.println("Turning Left");
    moveLeft();
  } 
  else if (digitalRead(irLeft) == 1 && digitalRead(irRight) == 0) {
    objectAvoid();
    Serial.println("Turning Right");
    moveRight();
  } 
  else if (digitalRead(irLeft) == 1 && digitalRead(irRight) == 1) {
    Stop();
  }
}

void objectAvoid() {
  distance = getDistance();
  if (distance <= 12) {
    Stop();
    Serial.println("Obstacle Detected");

    lookLeft();
    lookRight();
    delay(100);
    
    if (rightDistance <= leftDistance) {
      object = true;
      turn();  // Turn left
      Serial.println("Turning Left to Avoid");
    } else { 
      object = false;
      turn();  // Turn right
      Serial.println("Turning Right to Avoid");
    }
    delay(100);
  } 
  else {
    Serial.println("Moving Forward");
    moveForward();
  }
}

int getDistance() {
  delay(50);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 100; // Default distance if no object is detected
  return cm;
}

int lookLeft() {
  servo.write(150); // Look left
  delay(500);
  leftDistance = getDistance();
  delay(100);
  servo.write(90); // Reset to center
  Serial.print("Left Distance: ");
  Serial.println(leftDistance);
  return leftDistance;
}

int lookRight() {
  servo.write(30); // Look right
  delay(500);
  rightDistance = getDistance();
  delay(100);
  servo.write(90); // Reset to center
  Serial.print("Right Distance: ");
  Serial.println(rightDistance);
  return rightDistance;
}

void Stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void moveForward() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void moveBackward() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void turn() {
  if (object == false) { // Turning right
    Serial.println("Turn Right");
    moveLeft();
    delay(600);
    moveForward();
    delay(800);
    moveRight();
    delay(900);

    recoverLine();
  } 
  else { // Turning left
    Serial.println("Turn Left");
    moveRight();
    delay(700);
    moveForward();
    delay(800);
    moveLeft();
    delay(900);

    recoverLine();
  }
}

void recoverLine() {
  // Continue forward until either sensor detects the line
  while (digitalRead(irLeft) == 1 && digitalRead(irRight) == 1) {
    moveForward();
    delay(100);
  }

  // Fine-tune alignment based on sensor readings
  if (digitalRead(irLeft) == 0 && digitalRead(irRight) == 1) {
    moveLeft();
    delay(300);
  } else if (digitalRead(irLeft) == 1 && digitalRead(irRight) == 0) {
    moveRight();
    delay(300);
  }
  Serial.println("Line Recovered");
  moveForward();
}

void moveRight() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void moveLeft() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}