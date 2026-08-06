#include <Arduino.h>

enum RobotState {
  idle,
  movingForward,
  turningLeft,
  turningRight,
  stopped
};

RobotState robotState = idle;

#define STEP_PIN_LEFT 18
#define DIR_PIN_LEFT 19

#define STEP_PIN_RIGHT 21
#define DIR_PIN_RIGHT 22

#define TRIG_PIN 4
#define ECHO_PIN 5

const float safeDistance = 20.0;

volatile float latestDistance = 999.0;

// Helper function
void stepMotor(int stepPin)
{
  // One HIGH-to-LOW pulse moves the motor one step
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(500);

  digitalWrite(stepPin, LOW);
  delayMicroseconds(500);
}

// Choose which direction each motor rotates
void setMotorDirections(bool leftDirection, bool rightDirection)
{
  digitalWrite(DIR_PIN_LEFT, leftDirection);
  digitalWrite(DIR_PIN_RIGHT, rightDirection);
}

// Move both motors together
void moveBothMotors(int steps)
{
  for (int i = 0; i < steps; i++)
  {
    stepMotor(STEP_PIN_LEFT);
    stepMotor(STEP_PIN_RIGHT);
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
  // Reset trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send 10 microsecond trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Measure how long the echo pin stays HIGH
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return 999.0;
  }

  // Convert round-trip time into centimeters
  float distance = (duration * 0.0343) / 2;

  return distance;
}

// This function decides the robot state
void updateState()
{
  if (latestDistance < safeDistance)
  {
    robotState = turningLeft;
  }
  else
  {
    robotState = movingForward;
  }
}

// This function performs the current state
void executeState()
{
  switch (robotState)
  {
    case movingForward:
      moveForward(10);
      break;

    case turningLeft:
      turnLeft(50);
      break;

    case turningRight:
      turnRight(50);
      break;

    case stopped:
    case idle:
      // No movement
      break;
  }
}

// FreeRTOS sensor task
void sensorTask(void* parameter)
{
  while (true)
  {
    latestDistance = getDistance();

    Serial.print("Distance: ");
    Serial.print(latestDistance);
    Serial.println(" cm");

    // Only this task sleeps for 200 ms
    vTaskDelay(pdMS_TO_TICKS(200));
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

  // Sensor task:
  // HC-SR04
  //    ↓
  // latestDistance
  xTaskCreatePinnedToCore(
    sensorTask,       // Function to run
    "Sensor Task",    // Task name
    2048,             // Stack memory
    NULL,             // No parameters
    1,                // Priority
    NULL,             // No task handle
    0                 // Core 0
  );

  Serial.println("Sensor task started");
}

void loop()
{
  // Sensor task updates latestDistance
  // Main loop decides and controls motors
  updateState();
  executeState();

  delay(20);
}