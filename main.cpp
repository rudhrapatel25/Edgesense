#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

enum LightLevel {
  DARK, //(0)
  NORMAL, //(1)
  BRIGHT  //(2)
};

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

  display.setCursor(0,23);
  display.print("Temp: ");
  display.print(temp,1);
  display.println(" C");

  display.setCursor(0,37);
  display.print("Humidity: ");
  display.print(humidity,1);
  display.print("%");

  display.setCursor(0,50);
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

  display.display();
}

void updateLED() {

  switch (lightStatus){
    case BRIGHT: 
      digitalWrite(LED_PIN, LOW);
      break;
  
    case NORMAL:
      digitalWrite(LED_PIN, HIGH);
      break;

    case DARK: 
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
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

    updateOLED();
    updateLED();

  }

  delay(1000);
}