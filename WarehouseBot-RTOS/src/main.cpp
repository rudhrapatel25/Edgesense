#include <Arduino.h>

enum RobotState {
  IDLE,
  MOVING_FORWARD,
  TURNING_LEFT,
  TURNING_RIGHT,
  STOPPED
};

RobotState robotState = IDLE;

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

#define TRIG_PIN 4
#define ECHO_PIN 5

const float SAFE_DISTANCE = 20.0;
const int STEP_DELAY_US = 500;

void setMotorDirections(bool leftDirection, bool rightDirection)
{
  digitalWrite(DIR_PIN_LEFT, leftDirection);
  digitalWrite(DIR_PIN_RIGHT, rightDirection);
}

void stepBothMotorsOnce()
{
  digitalWrite(STEP_PIN_LEFT, HIGH);
  digitalWrite(STEP_PIN_RIGHT, HIGH);

  delayMicroseconds(STEP_DELAY_US);

  digitalWrite(STEP_PIN_LEFT, LOW);
  digitalWrite(STEP_PIN_RIGHT, LOW);

  delayMicroseconds(STEP_DELAY_US);
}

void moveBothMotors(int steps)
{
  for(int i = 0; i < steps; i++)
  {
    stepBothMotorsOnce();
  }
}

void moveForward(int steps)
{
  setMotorDirections(HIGH, LOW);
  moveBothMotors(steps);
}

void turnLeft(int steps)
{
  setMotorDirections(LOW, LOW);
  moveBothMotors(steps);
}

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if(duration == 0)
    return 999;

  return (duration * 0.0343) / 2.0;
}

void updateState(float distance)
{
  if(distance < SAFE_DISTANCE)
  {
    robotState = TURNING_LEFT;
  }
  else
  {
    robotState = MOVING_FORWARD;
  }
}

void executeState()
{
  switch(robotState)
  {
    case MOVING_FORWARD:
      moveForward(20);
      break;

    case TURNING_LEFT:
      turnLeft(80);
      break;

    case STOPPED:
    case IDLE:
      break;

    case TURNING_RIGHT:
      break;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(STEP_PIN_LEFT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  pinMode(STEP_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_RIGHT, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop()
{
  float distance = getDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  updateState(distance);

  executeState();
}