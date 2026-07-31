#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

enum LightLevel {
  DARK, //(0)
  NORMAL, //(1)
  BRIGHT  //(2)
};

enum EnvironmentStatus {
  HEALTHY,
  HOT,
  HUMID,
  DARK_ROOM
};

EnvironmentStatus roomStatus = HEALTHY;
LightLevel lightStatus = NORMAL;

#define SCREEN_WDH 128
#define SCREEN_HGT 64
Adafruit_SSD1306 display(SCREEN_WDH, SCREEN_HGT, &Wire, -1); //OLED object

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE); //DHT22 object

#define LDR_PIN 34
int lightValue = 0;

float temp = 0;
float humidity = 0;

#define LED_PIN 18


void updateOLED(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(11,0);
  display.println("EdgeSense");

  display.drawLine(0, 18, 127, 18, SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0,21);
  display.print("Temp: ");
  display.print(temp,1);
  display.println(" C");

  display.setCursor(0,32);
  display.print("Humidity: ");
  display.print(humidity,1);
  display.print("%");

  display.setCursor(0,43);
  display.print("Light: ");
  switch(lightStatus){
    case DARK: 
      display.print("Dark");
      break;

    case NORMAL:
      display.print("Normal");
      break;

    case BRIGHT:
      display.print("Bright");
      break;
  }

  display.setCursor(0, 54);
  display.print("Status: ");
  switch(roomStatus){
    case HEALTHY:
      display.print("Healthy:)");
      break;
   case HOT:
      display.print("HOT!!!");
      break;
    case HUMID:
      display.print("Humid:(");
      break;
    case DARK_ROOM:
      display.print("SPOOKY:/");
      break;
  }
  display.display();
}

unsigned long previousLEDTime = 0;
bool ledState = false;

void updateLED() {
  unsigned long currentTime = millis();

  switch (roomStatus){
    case HEALTHY: 
      digitalWrite(LED_PIN, LOW); //OFF
      break;
  
    case HOT: 
      digitalWrite(LED_PIN, HIGH); //ON
      break;

    case HUMID: //Slow blink every 1000ms
      if(currentTime - previousLEDTime >= 1000)
      {
        previousLEDTime = currentTime;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
      break;

    case DARK_ROOM: //Fast blink every 250ms
      if(currentTime - previousLEDTime >= 250)
      {
        previousLEDTime = currentTime;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
      break;

    default: //OFF for safety
      digitalWrite(LED_PIN, LOW);
      break;
  }
}

void updateLightStatus(){
  if(lightValue > 3000){
    lightStatus = DARK;
  } 
  else if (lightValue > 1000){
    lightStatus = NORMAL;
  } 
  else {
    lightStatus = BRIGHT;
  }
}

void updateEnvironmentStatus(){

  if(temp > 30){
    roomStatus = HOT;
  }
  else if(humidity > 70){
    roomStatus = HUMID;
  }
  else if(lightStatus == DARK){
    roomStatus = DARK_ROOM;
  } 
  else {
    roomStatus = HEALTHY;
  }
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("OLED failed");
    while(true);
  }

  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  delay(2000);
}

void loop() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  lightValue = analogRead(LDR_PIN);

  updateLightStatus();

  if (isnan(newTemp) || isnan(newHumidity)) {
    Serial.println("DHT22 reading failed");
  } 
  else {
    temp = newTemp;
    humidity = newHumidity;

    Serial.print("Temperature: ");
    Serial.println(temp);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    Serial.print("Light ADC: ");
    Serial.println(lightValue);

    updateEnvironmentStatus();
    updateOLED();
    updateLED();

  }

}