#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

#define screenwidth 128
#define screenheight 64

#define oled_sda 23
#define oled_scl 25

Adafruit_SSD1306 display(
  screenwidth,
  screenheight,
  &Wire,
  -1
);

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


// Returns text for the current robot state
const char* getRobotStateName()
{
  switch (robotState)
  {
    case idle:
      return "IDLE";

    case movingForward:
      return "FORWARD";

    case turningLeft:
      return "TURN LEFT";

    case turningRight:
      return "TURN RIGHT";

    case stopped:
      return "STOPPED";
  }

  return "UNKNOWN";
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
        break;
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}


// Display task shows the current robot information
void displayTask(void* parameter)
{
  while (true)
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(4, 0);
    display.println("Warehouse");

    display.setTextSize(1);
    display.setCursor(42, 16);
    display.println("Bot");

    display.drawLine(0, 27, 127, 27, SSD1306_WHITE);

    display.setCursor(0, 33);
    display.print("Distance: ");
    display.print(latestDistance, 1);
    display.println(" cm");

    display.setCursor(0, 44);
    display.print("State: ");
    display.println(getRobotStateName());

    display.setCursor(0, 56);
    display.print("RTOS: Running");

    display.display();

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

  // Start the OLED before creating displayTask
  Wire.begin(oled_sda, oled_scl);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED failed");

    while (true)
    {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.display();


  xTaskCreatePinnedToCore(
    sensorTask,
    "Sensor Task",
    2048,
    NULL,
    1,
    NULL,
    0
  );


  xTaskCreatePinnedToCore(
    navigationTask,
    "Navigation Task",
    2048,
    NULL,
    2,
    NULL,
    1
  );


  xTaskCreatePinnedToCore(
    motorTask,
    "Motor Task",
    2048,
    NULL,
    3,
    NULL,
    1
  );


  xTaskCreatePinnedToCore(
    displayTask,
    "Display Task",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  Serial.println("WarehouseBot display task started");
}


void loop()
{
  delay(1000);
}