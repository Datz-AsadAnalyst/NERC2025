// -----------------------------
// Encoder Motor Pins
#define LEFT_ENCODER_PIN 2
#define RIGHT_ENCODER_PIN 3

// -----------------------------
// PWM Motor Pins
int leftMotorForwardPwm = 5;
int leftMotorBackwardPwm = 6;
int rightMotorForwardPwm = 8;
int rightMotorBackwardPwm = 9;

// -----------------------------
// Sensor Pins - Front
int frontRightMost = A4;      
int frontRight = A6;
int frontMiddle =  A3;
int frontLeft = A7;
int frontLeftMost = A5;

// Sensor Pins - Back
int BackLeftMost = A9;
int BackLeft = A8;
int BackMiddle = A10;
int BackRight = A1;
int BackRightMost = A2;

// -----------------------------
// Encoder Tick Counters
volatile int rightEncoderTick = 0;
volatile int leftEncoderTick = 0;

// Line Following State Variables
bool is_line = false;
unsigned long prevMillis = 0;
const long interval = 100;

// Threshold for analog IR sensors
int threshold = 600;  

// -----------------------------
// Encoder Interrupts
void rightEncoderISR() {
  rightEncoderTick++;
}

void leftEncoderISR() {
  leftEncoderTick++;
}

// -----------------------------
// Setup Function
void setup() {
  Serial.begin(9600);

  // Encoder pin setup
  // pinMode(LEFT_ENCODER_PIN, INPUT);
  // pinMode(RIGHT_ENCODER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), leftEncoderISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), rightEncoderISR, RISING);

  // Motor pin setup
  pinMode(leftMotorForwardPwm, OUTPUT);
  pinMode(leftMotorBackwardPwm, OUTPUT);
  pinMode(rightMotorForwardPwm, OUTPUT);
  pinMode(rightMotorBackwardPwm, OUTPUT);

  // Sensor pin setup
  pinMode(backLeftMost, INPUT);
  pinMode(backLeft, INPUT);
  pinMode(backMiddle, INPUT);
  pinMode(backRight, INPUT);
  pinMode(backRightMost, INPUT);

  pinMode(frontLeftMost, INPUT);
  pinMode(frontLeft, INPUT);
  pinMode(frontMiddle, INPUT);
  pinMode(frontRight, INPUT);
  pinMode(frontRightMost, INPUT);

  lineFollowUntil(3);
  //lineFollow();
}

// -----------------------------
// Basic Movement Functions
void moveForward(int speed) {
  analogWrite(leftMotorForwardPwm, speed);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, speed);
  analogWrite(rightMotorBackwardPwm, 0);
}

void moveBackward(int speed) {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, speed);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, speed);
}

void turnLeft(int speed) {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, speed);
  analogWrite(rightMotorForwardPwm, speed);
  analogWrite(rightMotorBackwardPwm, 0);
}

void turnRight(int speed) {
  analogWrite(leftMotorForwardPwm, speed);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, speed);
}

void stopMotors() {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, 0);
}

// -----------------------------
// Encoder Movement Functions
void moveForwardEncoder(int targetTicks, int speed) {
  leftEncoderTick = 0;
  rightEncoderTick = 0;

  moveForward(speed);

  while (leftEncoderTick < targetTicks && rightEncoderTick < targetTicks) {
    Serial.print("Left Ticks: ");
    Serial.print(leftEncoderTick);
    Serial.print(" | Right Ticks: ");
    Serial.println(rightEncoderTick);
  }

  stopMotors();
}

void moveBackwardEncoder(int targetTicks, int speed) {
  leftEncoderTick = 0;
  rightEncoderTick = 0;

  moveBackward(speed);

  while (leftEncoderTick < targetTicks && rightEncoderTick < targetTicks) {
    Serial.print("Reverse -> Left: ");
    Serial.print(leftEncoderTick);
    Serial.print(" | Right: ");
    Serial.println(rightEncoderTick);
  }

  stopMotors();
}

void moveLeftEncoder(int targetTicks, int speed) {
  leftEncoderTick = 0;
  rightEncoderTick = 0;

  turnLeft(speed);

  while (leftEncoderTick < targetTicks && rightEncoderTick < targetTicks) {
    Serial.print("Left Turn -> Left: ");
    Serial.print(leftEncoderTick);
    Serial.print(" | Right: ");
    Serial.println(rightEncoderTick);
  }

  stopMotors();
}

void moveRightEncoder(int targetTicks, int speed) {
  leftEncoderTick = 0;
  rightEncoderTick = 0;

  turnRight(speed);

  while (leftEncoderTick < targetTicks && rightEncoderTick < targetTicks) {
    Serial.print("Right Turn -> Left: ");
    Serial.print(leftEncoderTick);
    Serial.print(" | Right: ");
    Serial.println(rightEncoderTick);
  }

  stopMotors();
}

// -----------------------------
// Sensor Reading Function
void readSensors() {
  int backLM = analogRead(backLeftMost);
  int backL = analogRead(backLeft);
  int backC = analogRead(backMiddle);
  int backR = analogRead(backRight);
  int backRM = analogRead(backRightMost);

  int frontLM = analogRead(frontLeftMost);
  int frontL = analogRead(frontLeft);
  int frontC = analogRead(frontMiddle);
  int frontR = analogRead(frontRight);
  int frontRM = analogRead(frontRightMost);

  Serial.println("Back Sensors:");
  Serial.print(backLM); Serial.print(" ");
  Serial.print(backL); Serial.print(" ");
  Serial.print(backC); Serial.print(" ");
  Serial.print(backR); Serial.print(" ");
  Serial.println(backRM);

  Serial.println("Front Sensors:");
  Serial.print(frontLM); Serial.print(" ");
  Serial.print(frontL); Serial.print(" ");
  Serial.print(frontC); Serial.print(" ");
  Serial.print(frontR); Serial.print(" ");
  Serial.println(frontRM);
}

// -----------------------------
// Line Following Function
void lineFollow() {
  int frontLM = analogRead(frontLeftMost) > threshold ? 1 : 0;
  int frontL = analogRead(frontLeft) > threshold ? 1 : 0;
  int frontC = analogRead(frontMiddle) > threshold ? 1 : 0;
  int frontR = analogRead(frontRight) > threshold ? 1 : 0;
  int frontRM = analogRead(frontRightMost) > threshold ? 1 : 0;

  if (frontL == 1 && frontC == 0 && frontR == 1) {
    moveForward(60);
  } else if ((frontL == 0 && frontC == 0 && frontR == 1) || (frontL == 0 && frontC == 1 && frontR == 1)) {
    turnLeft(60);
  } else if ((frontL == 1 && frontC == 0 && frontR == 0) || (frontL == 1 && frontC == 1 && frontR == 0)) {
    turnRight(60);
  } else if (!frontLM && !frontL && frontC && frontR && frontRM) {
    turnLeft(60);
  } else if (!frontLM && frontL && frontC && frontR && frontRM) {
    turnLeft(60);
  } else if (frontLM && !frontL && frontC && frontR && frontRM) {
    turnLeft(60);
  } else if (frontLM && frontL && frontC && !frontR && !frontRM) {
    turnRight(60);
  } else if (frontLM && frontL && frontC && frontR && !frontRM) {
    turnRight(60);
  } else if (frontLM && frontL && frontC && !frontR && frontRM) {
    turnRight(60);
  } else {
    stopMotors();
  }
}

// -----------------------------
// Line Follow Until Strip Count
void lineFollowUntil(int count) {
  int strips_count = 0;

  while (true) {
    int frontLM = analogRead(frontLeftMost) > threshold ? 1 : 0;
    int frontL = analogRead(frontLeft) > threshold ? 1 : 0;
    int frontC = analogRead(frontMiddle) > threshold ? 1 : 0;
    int frontR = analogRead(frontRight) > threshold ? 1 : 0;
    int frontRM = analogRead(frontRightMost) > threshold ? 1 : 0;

    if (!frontLM && !frontL && !frontC && !frontR && !frontRM && !is_line) {
      strips_count++;
      is_line = true;
      prevMillis = millis();
    } else if (millis() - prevMillis > interval && is_line) {
      is_line = false;
    }

    if (strips_count < count) {
      lineFollow();
      Serial.print("Strips: ");
      Serial.println(strips_count);
    } else {
      moveBackwardEncoder(50, 45);
      stopMotors();
      return;
    }
  }
}

// -----------------------------
// Main Loop
void loop() {
  // Empty — use setup() or call your logic here
}

