#include <Arduino.h>

enum RobotState {
  idle,
  movingForward,
  turningLeft,
  turningRight,
  stopped
};

volatile RobotState robotState = idle;

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

#define TRIG_PIN 4
#define ECHO_PIN 5

const float safeDistance = 20.0;

volatile float latestDistance = 999.0;

bool turnLeftNext = true;
bool obstacleHandled = false;


// Changes the direction of both motors
void setMotorDirections(bool leftDirection, bool rightDirection)
{
  digitalWrite(DIR_PIN_LEFT, leftDirection);
  digitalWrite(DIR_PIN_RIGHT, rightDirection);
}


// Sends one step pulse to both motors
void stepBothMotorsOnce()
{
  digitalWrite(STEP_PIN_LEFT, HIGH);
  digitalWrite(STEP_PIN_RIGHT, HIGH);

  delayMicroseconds(500);

  digitalWrite(STEP_PIN_LEFT, LOW);
  digitalWrite(STEP_PIN_RIGHT, LOW);

  delayMicroseconds(500);
}


// Reads the ultrasonic sensor
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

  float distance = (duration * 0.0343) / 2;

  return distance;
}


// Sensor task only reads distance
void sensorTask(void* parameter)
{
  while (true)
  {
    latestDistance = getDistance();

    Serial.print("Distance: ");
    Serial.print(latestDistance);
    Serial.println(" cm");

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


// Navigation task only decides what the robot should do
void navigationTask(void* parameter)
{
  while (true)
  {
    if (latestDistance < safeDistance)
    {
      if (!obstacleHandled)
      {
        if (turnLeftNext)
        {
          robotState = turningLeft;
          Serial.println("State: turning left");
        }
        else
        {
          robotState = turningRight;
          Serial.println("State: turning right");
        }

        turnLeftNext = !turnLeftNext;
        obstacleHandled = true;
      }
    }
    else
    {
      robotState = movingForward;
      obstacleHandled = false;
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


// Motor task reads robotState and moves the motors
void motorTask(void* parameter)
{
  while (true)
  {
    switch (robotState)
    {
      case movingForward:
        setMotorDirections(HIGH, LOW);
        stepBothMotorsOnce();
        break;

      case turningLeft:
        setMotorDirections(LOW, LOW);
        stepBothMotorsOnce();
        break;

      case turningRight:
        setMotorDirections(HIGH, HIGH);
        stepBothMotorsOnce();
        break;

      case stopped:
      case idle:
        // No STEP pulse means the motors stay stopped
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(1));
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


  // Sensor Task
  // HC-SR04
  //    ↓
  // latestDistance
  xTaskCreatePinnedToCore(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL,
    0
  );


  // Navigation Task
  // latestDistance
  //    ↓
  // robotState
  xTaskCreatePinnedToCore(
    navigationTask,
    "Navigation Task",
    2048,
    NULL,
    2,
    NULL,
    1
  );


  // Motor Task
  // robotState
  //    ↓
  // motors
  xTaskCreatePinnedToCore(
    motorTask,
    "Motor Task",
    2048,
    NULL,
    3,
    NULL,
    1
  );

  Serial.println("WarehouseBot RTOS tasks started");
}


void loop()
{
  // The FreeRTOS tasks handle everything now
  delay(1000);
}