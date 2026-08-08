#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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


//Helper function
void stepMotor(int stepPin){
  //That sequence creates one pulse; ONE pulse = one step
  //Move ONE motor exactly one step
  //Motor connected to this STEP pin, move one step
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(500);


  digitalWrite(stepPin, LOW);
  delayMicroseconds(500);
}


//Choose which direction each motor should rotate
//Think of it like shifting the car into Drive or Reverse
//In Arduino/C++, true = HIGH and false = LOW
void setMotorDirections(bool leftDirection, bool rightDirection){
  digitalWrite(DIR_PIN_LEFT, leftDirection);
  digitalWrite(DIR_PIN_RIGHT, rightDirection);
}


//Move both motors together for a certain number of steps
void moveBothMotors(int steps){
  //Both motors rotate together
  for(int i = 0; i < steps; i++){
    stepMotor(STEP_PIN_LEFT);
    stepMotor(STEP_PIN_RIGHT);
  }
}


void moveForward(int steps){
  setMotorDirections(HIGH, LOW);
  moveBothMotors(steps);
}


void moveBackward(int steps){
  setMotorDirections(LOW, HIGH);
  moveBothMotors(steps);
}


void turnLeft(int steps){
  setMotorDirections(LOW, LOW);
  moveBothMotors(steps);
}


void turnRight(int steps){
  setMotorDirections(HIGH, HIGH);
  moveBothMotors(steps);
}


float getDistance(){


  //Reset the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);


  //Send a 10 µs trigger pulse (Start measuring)
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);


  //End the trigger pulse
  digitalWrite(TRIG_PIN, LOW);


  //Wait for the echo and measure how long the ECHO pin stays HIGH (Round trip time)
  long duration = pulseIn(ECHO_PIN, HIGH);


  //Convert the measurted time into centimeters
  float distance = (duration * 0.0343) / 2;


  return distance;
}


void stopRobot(){
}

const unsigned long stopTime = 300;
const unsigned long turnTime = 900;

unsigned long stateStartTime = 0;

bool turnLeftNext = true;
/*This function only thinks
//It never moves a motor
//Each time an obstacle is detected:
          //First obstacle = turn left
          //Next obstacle = turn right
          //Next obstacle = turn left
void updateState(){
  //float distance = getDistance();


  //Sensor Task (Sensor keeps updating distance every 200ms
                //Meanwhile main loop keeps deciding and controlling motors
  //reads distance every 200 ms
      //↓
  //latestDistance
      //↓
  //Arduino loop updates state and controls motors


  if(latestDistance < safeDistance){
    if(turnLeftNext){
    robotState = turningLeft;
    }
    else {
      robotState = turningRight;
    }
    turnLeftNext = !turnLeftNext;
    obstacleHandled = true;
  }
  else {
    robotState = movingForward;
  }
}*/


/*void executeState(){
  switch(robotState){
    case movingForward:
      Serial.println("State: Moving forward");
      moveForward(10);
      break;


    case turningLeft:
      Serial.println("State: Turning left");
      turnLeft(50);
      break;


    case turningRight:
      Serial.println("State: Turning right");
      stopRobot();
      delay(200);
      turnRight(50);
      break;


    case stopped:
      Serial.println("State: Stopped");
      stopRobot();
      break;


    case idle:
      Serial.println("State: Idle");
      stopRobot();
      break;
  }
}*/


void sensorTask(void *parameter){


  //required FreeRTOS task format
  while(true){
    latestDistance = getDistance();


    Serial.print("Distance: ");
    Serial.print(latestDistance);
    Serial.println(" cm");


    //Only sensor tasks sleeps for 200ms. Other tasks run
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


void stepBothMotorsOnce(){
  digitalWrite(STEP_PIN_LEFT, HIGH);
  digitalWrite(STEP_PIN_RIGHT, HIGH);


  delayMicroseconds(500);


  digitalWrite(STEP_PIN_LEFT, LOW);
  digitalWrite(STEP_PIN_RIGHT, LOW);


  delayMicroseconds(500);
}


//Every 20ms, check the current robot state and perform the matching
//motor action
void motorTask(void *parameter){
  while(true)
  {
    switch(robotState)
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
        //No STEP pulse means the motors do not move
        break;
    }


    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

const char* getRobotStateName(){
  switch(robotState)
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

void changeRobotState(RobotState newState){
  if(robotState != newState){
    robotState = newState;
    stateStartTime = millis();

    Serial.print("State changed: ");
    Serial.println(getRobotStateName());
  }
}

void navigationTask(void* parameter){
  while(true){
    unsigned long currentTime = millis();

    switch(robotState){
      case idle: 
        changeRobotState(movingForward);
        break;

      case movingForward:
        if(latestDistance < safeDistance){
          changeRobotState(stopped);
        }
        break;

      case stopped: 
        if(currentTime - stateStartTime >= stopTime){
          if(turnLeftNext){
            changeRobotState(turningLeft);
          }

          else{
            changeRobotState(turningRight);
          }

          turnLeftNext = !turnLeftNext;
        }

        break;

        case turningLeft: 
        case turningRight:
          if(currentTime - stateStartTime >= turnTime){
            changeRobotState(movingForward);
          }
          break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
  }
}


void displayTask(void *parameter){
  while(true){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);


    display.setTextSize(2);
    display.setCursor(4, 0);
    display.println("Warehouse");


    display.setTextSize(1);
    display.setCursor(42,16);
    display.println("Bot:)");


    display.drawLine(0, 27, 127, 27, SSD1306_WHITE);


    display.setCursor(0, 33);
    display.print("Distance: ");
    display.print(latestDistance, 1);
    display.println(" cm");

    display.setCursor(0, 44);
    display.print("State: ");
    display.println(getRobotStateName());


    display.setCursor(0,55);
    display.print("RTOS: Running");


    display.display();


    vTaskDelay(pdMS_TO_TICKS(200)); //Updates every 200ms but
    //does not control the robot
  }
}


void setup() {
  Serial.begin(115200);


  pinMode(STEP_PIN_LEFT, OUTPUT);
  pinMode(DIR_PIN_LEFT, OUTPUT);


  pinMode(STEP_PIN_RIGHT, OUTPUT);
  pinMode(DIR_PIN_RIGHT, OUTPUT);


  digitalWrite(DIR_PIN_LEFT, HIGH);
  digitalWrite(DIR_PIN_RIGHT, HIGH);


  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  //Display
  Wire.begin(oled_sda, oled_scl);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED failed");
  }

//Sensor Task
//reads HC-SR04
  //   ↓
//latestDistance


//Navigation Task
//reads latestDistance
  //  ↓
//robotState


//Motor Task
//reads robotState
    //↓
//moves motors
  xTaskCreatePinnedToCore(
    sensorTask,         //Function to run
    "Sensor Task",      //Task name
    2048,               //Stack memory
    NULL,               //No parameters
    1,                  //Priority
    NULL,               //No task handle
    0                   // Core 0
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


  //The motor task gets the higher priority on Core 1
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
}

void loop() {
}