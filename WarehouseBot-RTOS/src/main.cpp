#include <Arduino.h>

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

#define TRIG_PIN 4
#define ECHO_PIN 5

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

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return 999.0;
  }

  float distance = (duration * 0.0343f) / 2.0f;

  return distance;
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

  Serial.println("WarehouseBot distance sensor test");
}

void loop()
{
  float distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}