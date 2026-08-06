#include <Arduino.h>

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

const int STEP_DELAY_US = 500;

// Move BOTH motors exactly one step
void stepBothMotorsOnce()
{
  digitalWrite(STEP_PIN_LEFT, HIGH);
  digitalWrite(STEP_PIN_RIGHT, HIGH);

  delayMicroseconds(STEP_DELAY_US);

  digitalWrite(STEP_PIN_LEFT, LOW);
  digitalWrite(STEP_PIN_RIGHT, LOW);

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

  Serial.println("Synchronized motor test");
}

void loop()
{
  stepBothMotorsOnce();
}