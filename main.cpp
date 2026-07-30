#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

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
  display.print(getLightStatus(lightValue));

  display.display();
}

const char* getLightStatus(int value) {
  if (value > 3000) {
    return "Dark";
  } 
  else if (value > 1000) {
    return "Normal";
  } 
  else {
    return "Bright";
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

  delay(2000);
}

void loop() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  lightValue = analogRead(LDR_PIN);

  if (isnan(newTemp) || isnan(newHumidity)) {
    Serial.println("DHT22 reading failed");
  } else {
    temp = newTemp;
    humidity = newHumidity;

    Serial.print("Temperature: ");
    Serial.println(temp);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    Serial.print("Light ADC: ");
    Serial.println(lightValue);

    updateOLED();
  }

  delay(5000);
}
