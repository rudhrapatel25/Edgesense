#include <Arduino.h>

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

const int STEP_DELAY_US = 500;

// Set direction for both motors
void setMotorDirections(bool leftDirection, bool rightDirection)
{
  digitalWrite(DIR_PIN_LEFT, leftDirection);
  digitalWrite(DIR_PIN_RIGHT, rightDirection);
}

// Move both motors exactly one step
void stepBothMotorsOnce()
{
  digitalWrite(STEP_PIN_LEFT, HIGH);
  digitalWrite(STEP_PIN_RIGHT, HIGH);

  delayMicroseconds(STEP_DELAY_US);

  digitalWrite(STEP_PIN_LEFT, LOW);
  digitalWrite(STEP_PIN_RIGHT, LOW);

  delayMicroseconds(STEP_DELAY_US);
}

// Move both motors for a specific number of steps
void moveBothMotors(int steps)
{
  for (int i = 0; i < steps; i++)
  {
    stepBothMotorsOnce();
  }
}

void moveForward(int steps)
{
  setMotorDirections(HIGH, LOW);
  moveBothMotors(steps);
}

void moveBackward(int steps)
{
  setMotorDirections(LOW, HIGH);
  moveBothMotors(steps);
}

void turnLeft(int steps)
{
  setMotorDirections(LOW, LOW);
  moveBothMotors(steps);
}

void turnRight(int steps)
{
  setMotorDirections(HIGH, HIGH);
  moveBothMotors(steps);
}

void setup()
{
  Serial.begin(115200);

  pinMode(STEP_PIN_LEFT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  pinMode(STEP_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_RIGHT, OUTPUT);

  Serial.println("WarehouseBot movement test");

  moveForward(200);
  delay(1000);

  moveBackward(200);
  delay(1000);

  turnLeft(100);
  delay(1000);

  turnRight(100);
}

void loop()
{
}