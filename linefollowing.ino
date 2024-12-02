#include <AFMotor.h> 

// Define motor pins
AF_DCMotor motorLeft(1);  // Motor 1
AF_DCMotor motorRight(2); // Motor 2

// Define IR sensor pins
#define sensor1 A0
#define sensor2 A1
#define sensor3 A2  // Center sensor
#define sensor4 A3
#define sensor5 A4

// Variables for lost line detection
int lostLineCounter = 0;
int lostLineThreshold = 100; // Adjust as needed

// Variables for dynamic delay based on sensor readings
int turnDelay = 200;  // Default delay for sharp turns

void setup() {
  Serial.begin(9600);

  // Set initial motor speeds
  motorLeft.setSpeed(150);   
  motorRight.setSpeed(150);  
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

void loop() {
  // Read sensor values
  int s1 = analogRead(sensor1);
  int s2 = analogRead(sensor2);
  int s3 = analogRead(sensor3);
  int s4 = analogRead(sensor4);
  int s5 = analogRead(sensor5);

  // Print sensor values to monitor in the Serial Monitor
  Serial.print("S1: "); Serial.print(s1);
  Serial.print(" S2: "); Serial.print(s2);
  Serial.print(" S3: "); Serial.print(s3);
  Serial.print(" S4: "); Serial.print(s4);
  Serial.print(" S5: "); Serial.println(s5);

  // Define the threshold to detect the black line
  int threshold = 506;  // Increased threshold for stability

  // Determine if sensors are detecting the black line
  bool onLine1 = s1 < threshold;
  bool onLine2 = s2 < threshold;
  bool onLine3 = s3 < threshold;  // Center sensor
  bool onLine4 = s4 < threshold;
  bool onLine5 = s5 < threshold;

  // Check if any sensor is on the line
  bool anyOnLine = onLine1 || onLine2 || onLine3 || onLine4 || onLine5;

  // Reset lost line counter if any sensor detects the line
  if (anyOnLine) {
    lostLineCounter = 0;
  } else {
    lostLineCounter++;
  }

  // Stop the robot if the line is lost for too long and attempt recovery
  if (lostLineCounter > lostLineThreshold) {
    stopMotors();
    Serial.println("Line lost. Stopping and attempting recovery.");
    // Attempt to recover by rotating in place
    attemptRecovery();
    return;
  }

  // Dynamic delay adjustment based on sensor input for sharp turns
  if (onLine1 || onLine5) {
    turnDelay = map(s1 + s5, 0, 1023 * 2, 150, 300);  // Map combined leftmost and rightmost sensor readings to adjust delay
  }

  // Movement decisions based on sensor input
  if (onLine3) {
    // Center sensor detects the line, move forward
    moveForward(150); // Slight increase in forward speed for better stability
  } 
  else if (onLine4 && !onLine5) {
    // Right-center sensor detects the line, slight right turn
    turnRightSlight(120, 80);  // Reduced difference in speeds for smoother correction
  } 
  else if (onLine5) {
    // Rightmost sensor detects the line, sharp right turn
    turnRightSharp(150, 80); 
  } 
  else if (onLine2 && !onLine1) {
    // Left-center sensor detects the line, slight left turn
    turnLeftSlight(80, 120);  // Reduced difference in speeds for smoother correction
  } 
  else if (onLine1) {
    // Leftmost sensor detects the line, sharp left turn
    turnLeftSharp(80, 150); 
  } 
  else {
    // Stop the robot if no sensors detect the line
    stopMotors();
    Serial.println("No line detected. Stopping.");
  }
}

void moveForward(int speed) {
  motorLeft.setSpeed(speed);   
  motorRight.setSpeed(speed);  
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

// Adjusted turns for smoother motion
void turnLeftSlight(int leftSpeed, int rightSpeed) {
  motorLeft.setSpeed(leftSpeed);   
  motorRight.setSpeed(rightSpeed);  
  motorLeft.run(BACKWARD);
  motorRight.run(FORWARD);  // Keep both motors moving forward for smoother turn
}

void turnRightSlight(int leftSpeed, int rightSpeed) {
  motorLeft.setSpeed(leftSpeed);   
  motorRight.setSpeed(rightSpeed);  
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);  // Keep both motors moving forward for smoother turn
}

void turnLeftSharp(int leftSpeed, int rightSpeed) {
  motorLeft.setSpeed(leftSpeed);   
  motorRight.setSpeed(rightSpeed);  
  motorLeft.run(BACKWARD);  
  motorRight.run(FORWARD);
  
  delay(turnDelay);  // Adjust the delay dynamically based on sensor readings
  motorLeft.setSpeed(60);  // Lower speed after sharp turn for better control
  motorRight.setSpeed(60); 
}

void turnRightSharp(int leftSpeed, int rightSpeed) {
  motorLeft.setSpeed(leftSpeed);   
  motorRight.setSpeed(rightSpeed);  
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);
  
  delay(turnDelay);  // Adjust the delay dynamically based on sensor readings
  motorLeft.setSpeed(60);  // Lower speed after sharp turn for better control
  motorRight.setSpeed(60); 
}

// Stop the motors
void stopMotors() {
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

// Attempt recovery by rotating in place to search for the line
void attemptRecovery() {
  // Spin right slowly to try to find the line again
  motorLeft.setSpeed(60);   
  motorRight.setSpeed(60);  
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);

  delay(400);  // Adjust delay as needed for recovery rotation
  stopMotors();  // Stop after attempt
