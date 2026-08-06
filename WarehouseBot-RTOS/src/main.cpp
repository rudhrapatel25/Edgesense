#include <Arduino.h>

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

const int STEP_DELAY_US = 500;

void stepMotor(int stepPin)
{
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(STEP_DELAY_US);

  digitalWrite(stepPin, LOW);
  delayMicroseconds(STEP_DELAY_US);
}

void setup()
{
  Serial.begin(115200);

  pinMode(STEP_PIN_LEFT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);

  pinMode(STEP_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_RIGHT, OUTPUT);

  digitalWrite(DIR_PIN_LEFT, HIGH);
  digitalWrite(DIR_PIN_RIGHT, HIGH);

  Serial.println("WarehouseBot dual motor test started");
}

void loop()
{
  stepMotor(STEP_PIN_LEFT);
  stepMotor(STEP_PIN_RIGHT);
}