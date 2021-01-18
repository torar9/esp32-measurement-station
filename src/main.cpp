#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_BME680.h>
#include "config.hpp"
#include "storage.hpp"
#include "sps30.hpp"
#include "rtcmodule.hpp"

WiFiClient wifiClient;
PubSubClient mqClient(wifiClient);
DHT dhtSensor(DHT_PIN, DHT_TYPE);
RTC_DS3231 rtc;
Adafruit_BME680 bme;
SDFS card(SD);

struct measurments
{
  int humidity;
  char* time;
  double temperature;
  double pressure;
  double altitude;
  double batteryLevel;
  sps30_measurement spsData;
};

void callback(char* topic, byte* message, unsigned int length);
void test(measurments &data);
void measure(measurments &data)
{
  data.temperature = bme.readTemperature();
  data.humidity = bme.readHumidity();
  data.altitude = bme.readAltitude(1013.25);
  data.pressure = bme.readPressure() / 100.0;
  data.time = RTCGetString(rtc);
  sps30ReadNewData(data.spsData);
}

void setup() 
{
  Serial.begin(9600);
  CardPrepare(card, SD_CS);
  sps30Prepare();
  sps30_stop_measurement();

  if(!bme.begin())
  {
    DBG_PRINTLN("Unable to init BME680!");
  }

  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
      setupOTA();

      mqClient.setServer(mqtt_server, 1883);
      mqClient.setCallback(callback);
  }

  RTCPrepare(rtc);

  if(rtc.lostPower() && WiFi.status() == WL_CONNECTED)
  {
    struct tm timeStr;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getLocalTime(&timeStr);

    RTCSetTimeOnline(rtc, timeStr);
  }

  esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP);
}

void loop() 
{
  measurments data;

  measure(data);
  test(data);

  esp_light_sleep_start();
}

void callback(char* topic, byte* message, unsigned int length)
{
  DBG_PRINTLN();
  Serial.print(F("Callback function"));
}

void test(measurments &data)
{
  DBG_PRINT("temperature: ");
  DBG_PRINT(data.temperature);
  DBG_PRINT(" °C");
  DBG_PRINT(" humidity: "); 
  DBG_PRINT(data.humidity);
  DBG_PRINT(" %");
  DBG_PRINT(" pressure: "); 
  DBG_PRINT(data.pressure);
  DBG_PRINT(" altitude: "); 
  DBG_PRINTLN(data.altitude);

  DBG_PRINT(" mc1p0: "); 
  DBG_PRINT(data.spsData.mc_1p0);
  DBG_PRINT(" mc2p5: "); 
  DBG_PRINT(data.spsData.mc_2p5);
  DBG_PRINT(" mc4p0: "); 
  DBG_PRINT(data.spsData.mc_4p0);
  DBG_PRINT(" mc10p0: "); 
  DBG_PRINT(data.spsData.mc_10p0);
  DBG_PRINT(" nc0p5: "); 
  DBG_PRINT(data.spsData.nc_0p5);
  DBG_PRINT(" nc1p0: "); 
  DBG_PRINT(data.spsData.nc_1p0);
  DBG_PRINT(" nc2p5: "); 
  DBG_PRINT(data.spsData.nc_2p5);
  DBG_PRINT(" nc4p0: "); 
  DBG_PRINT(data.spsData.nc_4p0);
  DBG_PRINT(" nc10p0: "); 
  DBG_PRINT(data.spsData.nc_10p0);
  DBG_PRINT(" partSize: "); 
  DBG_PRINTLN(data.spsData.typical_particle_size);

  DBG_PRINTLN(data.time);
  DBG_PRINTLN();

  Serial.flush();
}