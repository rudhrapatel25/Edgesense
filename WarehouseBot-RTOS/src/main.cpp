#include <Arduino.h>

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

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

  digitalWrite(DIR_PIN_LEFT, HIGH);

  Serial.println("WarehouseBot motor test started");
}

void loop()
{
  stepMotor(STEP_PIN_LEFT);
}