#include "config.h"
#include <ArduinoUniqueID.h>

// Temp/Hum Sensor
#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

// Graphics
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Adafruit.io
AdafruitIO_Feed *temperature;
AdafruitIO_Feed *humidity;
int reportCounter = 0;



int transmitInterval = 60;

void setup() {
  // ======== SERIAL ========
  Serial.begin(115200);
  waitForSerial(3000);

  byte mac[6];
  WiFi.macAddress(mac);
  String uniq =  String(mac[0],HEX) +String(mac[1],HEX) +String(mac[2],HEX) +String(mac[3],HEX) + String(mac[4],HEX) + String(mac[5],HEX);
  Serial.println("ENVMON-" + uniq + "-TEMP");
  String tempName = "ENVMON-" + uniq + "-TEMP";
  String humName = "ENVMON-" + uniq + "-HUM";
  
  // Assemble the feed names from the serial numbers
  temperature = io.feed(tempName.c_str());
  Serial.println("Reporting Temperature at: " + tempName);
  humidity = io.feed(humName.c_str());
  Serial.println("Reporting Humidity at:    " + humName);

  // ======== TFT ========
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  // ======== SHT4x Temp/Humidity Sensor ========
  if (!sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);

  // ======== IO.ADAFRUIT.COM ========
  // connect to io.adafruit.com
  Serial.println("Connecting to Adafruit IO");
  io.connect();
}

void loop() {
  io.run();

  sensors_event_t eventHumidity, eventTemp;
  
  uint32_t timestamp = millis();
  sht4.getEvent(&eventHumidity, &eventTemp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp;

  float celsius = eventTemp.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  Serial.print(fahrenheit);
  Serial.print("F, ");

  Serial.print(eventHumidity.relative_humidity);
  Serial.println("%");

  printScreen(fahrenheit, eventHumidity.relative_humidity);

  if (io.status() == AIO_CONNECTED) {
    if (++reportCounter % transmitInterval == 0) {
      Serial.print("reporting to IO");
      temperature->save(fahrenheit);
      humidity->save(eventHumidity.relative_humidity);
      reportCounter = 0;
    }
  }

  delay(1000);
}

void printScreen(float temperature, float humidity) {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0, 0);
  if (io.status() == AIO_CONNECTED) {
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.print("WiFi Connected   ");
    tft.print((transmitInterval - reportCounter));
  } else {
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(1);
    tft.println("WiFi Disconnected");
  }
  
  tft.setCursor(0, 25);
  if (temperature < minTemp || temperature > maxTemp) {
    tft.setTextColor(ST77XX_RED);
  } else {
    tft.setTextColor(ST77XX_GREEN);
  }
  tft.setTextSize(5);
  tft.print(temperature);
  tft.println(" F");

  tft.setCursor(0, 90);
  if (humidity < minHum || humidity > maxHum) {
    tft.setTextColor(ST77XX_RED);
  } else {
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.print(humidity);
  tft.println(" %");
}

void waitForSerial(unsigned long timeout_millis) {
  unsigned long start = millis();
  while (!Serial) {
    if (millis() - start > timeout_millis)
      break;
  }
}
