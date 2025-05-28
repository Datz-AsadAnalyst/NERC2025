// -----------------------------
// Encoder Motor Pins
#define LEFT_ENCODER_PIN 2
#define RIGHT_ENCODER_PIN 3

// -----------------------------
// PWM Motor Pins
int leftMotorForwardPwm = 6;
int leftMotorBackwardPwm = 7;
int rightMotorForwardPwm = 5;
int rightMotorBackwardPwm = 4;

// -----------------------------
// Sensor Pins - Back

int backLeftMost = A4;
int backLeft = A0;
int backMiddle = A3;
int backRight = A1;
int backRightMost = A2;
// Sensor Pins - Front

int frontRightMost = A8;
int frontRight = A9;
int frontMiddle = A7;
int frontLeft = A6;
int frontLeftMost = A5;

// -----------------------------
// Encoder Tick Counters
volatile int rightEncoderTick = 0;
volatile int leftEncoderTick = 0;

// Line Following State Variables
bool is_line = false;
unsigned long prevMillis = 0;
const long interval = 100;

// Threshold for analog IR sensors
int threshold = 700;

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

  // lineFollowUntil_back(3);
}

// -----------------------------
// Basic Movement Functions
// Move forward
void moveForward(int speed) {
  analogWrite(leftMotorForwardPwm, speed);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, speed);
  analogWrite(rightMotorBackwardPwm, 0);
}

// Move backward
void moveBackward(int speed) {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, speed);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, speed);
}

// Turn left in place
void turnLeft() {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, 25);
  analogWrite(rightMotorForwardPwm, 50);
  analogWrite(rightMotorBackwardPwm, 0);
}

// Turn right in place
void turnRight() {
  analogWrite(leftMotorForwardPwm, 25);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, 25);
}

// Slight left (curve)
void softLeft() {
  analogWrite(leftMotorForwardPwm, 50);  // Slow left motor
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 100);  // Fast right motor
  analogWrite(rightMotorBackwardPwm, 0);
}

// Slight right (curve)
void softRight() {
  analogWrite(leftMotorForwardPwm, 100);  // Fast left motor
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 50);  // Slow right motor
  analogWrite(rightMotorBackwardPwm, 0);
}

// Turn 90 degrees right (in place)
void turnRight90(int speed) {
  analogWrite(leftMotorForwardPwm, speed);
  analogWrite(leftMotorBackwardPwm, 0);
  analogWrite(rightMotorForwardPwm, 0);
  analogWrite(rightMotorBackwardPwm, speed);
}

// Turn 90 degrees left (in place)
void turnLeft90(int speed) {
  analogWrite(leftMotorForwardPwm, 0);
  analogWrite(leftMotorBackwardPwm, speed);
  analogWrite(rightMotorForwardPwm, speed);
  analogWrite(rightMotorBackwardPwm, 0);
}

// Stop both motors
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

  turnLeft();

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

  turnRight();

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
void readFrontSensors() {
  int frontLM = analogRead(frontLeftMost);
  int frontL = analogRead(frontLeft);
  int frontC = analogRead(frontMiddle);
  int frontR = analogRead(frontRight);
  int frontRM = analogRead(frontRightMost);

  Serial.print(frontLM);
  Serial.print("\t");
  Serial.print(frontL);
  Serial.print("\t");
  Serial.print(frontC);
  Serial.print("\t");
  Serial.print(frontR);
  Serial.print("\t");
  Serial.println(frontRM);
}

void readBackSensors() {
  int backLM = analogRead(backLeftMost);
  int backL = analogRead(backLeft);
  int backC = analogRead(backMiddle);
  int backR = analogRead(backRight);
  int backRM = analogRead(backRightMost);
  //   int backLM = analogRead(backLeftMost) > threshold ? 1 : 0;
  // int backL = analogRead(backLeft) > threshold ? 1 : 0;
  // int backC = analogRead(backMiddle) > threshold ? 1 : 0;
  // int backR = analogRead(backRight) > threshold ? 1 : 0;
  // int backRM = analogRead(backRightMost) > threshold ? 1 : 0;

  Serial.print(backLM);
  Serial.print(" ");
  Serial.print(backL);
  Serial.print(" ");
  Serial.print(backC);
  Serial.print(" ");
  Serial.print(backR);
  Serial.print(" ");
  Serial.println(backRM);
}
//////Front
void lineFollowfront() {
  int frontLM = analogRead(frontLeftMost) > threshold ? 1 : 0;
  int frontL = analogRead(frontLeft) > threshold ? 1 : 0;
  int frontC = analogRead(frontMiddle) > threshold ? 1 : 0;
  int frontR = analogRead(frontRight) > threshold ? 1 : 0;
  int frontRM = analogRead(frontRightMost) > threshold ? 1 : 0;
  if (frontL == 0 && frontC == 0 && frontR == 0) {
    moveForward(50);
  } else if (frontL == 1 && frontC == 0 && frontR == 1) {
    moveForward(50);
    readFrontSensors();
    Serial.print("first");
  } else if (frontL == 0 && frontC == 0 && frontR == 1) {
    turnLeft();
    readFrontSensors();
    Serial.print("2nd");
  } else if (frontL == 0 && frontC == 1 && frontR == 1) {
    turnLeft();
    readFrontSensors();
    Serial.print("3rd");
  } else if (frontL == 1 && frontC == 0 && frontR == 0) {
    turnRight();
    readFrontSensors();
    Serial.print("4th");
  } else if (frontL == 1 && frontC == 1 && frontR == 0) {
    turnRight();
    readFrontSensors();
    Serial.print("5th");
  }

  // // 00111
  else if (!frontLM && !frontL && frontC && frontR && frontRM) {
    turnLeft();
    readFrontSensors();
    Serial.print("6th");
  }
  // 01111
  else if (!frontLM && frontL && frontC && frontR && frontRM) {
    turnLeft();
    readFrontSensors();
    Serial.print("7th");
  }
  // 10111
  else if (frontLM && !frontL && frontC && frontR && frontRM) {
    turnLeft();
    readFrontSensors();
    Serial.print("8th");
  }
  // 11100
  else if (frontLM && frontL && frontC && !frontR && !frontRM) {
    turnRight();
    readFrontSensors();
    Serial.print("9th");
  }
  // 11110
  else if (frontLM && frontL && frontC && frontR && !frontRM) {
    turnRight();
    readFrontSensors();
    Serial.print("10th");
  }
  // 11101
  else if (frontLM && frontL && frontC && !frontR && frontRM) {
    turnRight();
    readFrontSensors();
    Serial.print("11th");
  }
}


/////-------------
void lineFollowback() {
  int backLM = analogRead(backLeftMost) > threshold ? 1 : 0;
  int backL = analogRead(backLeft) > threshold ? 1 : 0;
  int backC = analogRead(backMiddle) > threshold ? 1 : 0;
  int backR = analogRead(backRight) > threshold ? 1 : 0;
  int backRM = analogRead(backRightMost) > threshold ? 1 : 0;

 if (backL == 0 && backC == 0 && backR == 0) {
    moveBackward(50);
    // Serial.println("backward");
  } else if (backL == 1 && backC == 0 && backR == 1) {
    moveBackward(50);
    // Serial.println("backward");
  } else if (backL == 1 && backC == 1 && backR == 0) {
    turnRight();
    // Serial.println("right");
  } else if (backL == 1 && backC == 0 && backR == 0) {
    turnRight();
    // Serial.println("right");
  } else if (backL == 0 && backC == 1 && backR == 1) {
    turnLeft();
    // Serial.println("left");
  } else if (backL == 0 && backC == 0 && backR == 1) {
    turnLeft();
    // Serial.println("left");
  } else {
    moveBackward(50);
    // Serial.println("backward");
  }

  // // // 00111
  // else if (!backLM && !backL && backC && backR && backRM) {
  //   turnRight();
  //   Serial.print("6th");
  // }
  // // 01111
  // else if (!backLM && backL && backC && backR && backRM) {
  //   turnRight();
  //   Serial.print("7th");
  // }
  // // 10111
  // else if (backLM && !backL && backC && backR && backRM) {
  //   turnRight();
  //   Serial.print("8th");
  // }
  // // 11100
  // else if (backLM && backL && backC && !backR && !backRM) {
  //   turnLeft();
  //   Serial.print("9th");
  // }
  // // 11110
  // else if (backLM && backL && backC && backR && !backRM) {
  //   turnLeft();
  //   Serial.print("10th");
  // }
  // // 11101
  // else if (backLM && backL && backC && !backR && backRM) {
  //   turnLeft();
  //   Serial.print("11th");
  // }
}


// -----------------------------
// Line Follow Until Strip Count
void lineFollowUntil_front(int count) {
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
      lineFollowfront();
      Serial.print("Strips: ");
      Serial.println(strips_count);
    } else {

      stopMotors();
      return;
    }
  }
}
/////////back array
void lineFollowUntil_back(int count) {
  int strips_count = 0;

  while (true) {
    int backLM = analogRead(backLeftMost) > threshold ? 1 : 0;
    int backL = analogRead(backLeft) > threshold ? 1 : 0;
    int backC = analogRead(backMiddle) > threshold ? 1 : 0;
    int backR = analogRead(backRight) > threshold ? 1 : 0;
    int backRM = analogRead(backRightMost) > threshold ? 1 : 0;

    if (!backLM && !backL && !backC && !backR && !backRM && !is_line) {
      strips_count++;
      is_line = true;
      prevMillis = millis();
    } else if (millis() - prevMillis > interval && is_line) {
      is_line = false;
    }

    if (strips_count < count) {
      lineFollowback();
      Serial.print("Strips: ");
      Serial.println(strips_count);
    } else {

      stopMotors();
      return;
    }
  }
}

// -----------------------------
// Main Loop
void loop() {
  // Empty — use setup() or call your logic here
  //   lineFollow();
  //   delay(100);
  // readBackSensors();
  // //  stopMotors();
  // moveForward(25);
  lineFollowback();
}
