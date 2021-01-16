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

void callback(char* topic, byte* message, unsigned int length);
void test();

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
  test();
  esp_light_sleep_start();
}

void callback(char* topic, byte* message, unsigned int length)
{
  DBG_PRINTLN();
  Serial.print(F("Callback function"));
}

void test()
{
  float temp = bme.readTemperature();
  float hum = bme.readHumidity();
  float altitude = bme.readAltitude(1013.25);
  float pressure = bme.readPressure();
  struct sps30_measurement spsData;
  sps30ReadNewData(spsData);

  DBG_PRINT("temperature: ");
  DBG_PRINT(temp);
  DBG_PRINT(" °C");
  DBG_PRINT(" humidity: "); 
  DBG_PRINT(hum);
  DBG_PRINT(" %");
  DBG_PRINT(" pressure: "); 
  DBG_PRINT(pressure / 100.0);
  DBG_PRINT(" altitude: "); 
  DBG_PRINTLN(altitude);

  DBG_PRINT(" mc1p0: "); 
  DBG_PRINT(spsData.mc_1p0);
  DBG_PRINT(" mc2p5: "); 
  DBG_PRINT(spsData.mc_2p5);
  DBG_PRINT(" mc4p0: "); 
  DBG_PRINT(spsData.mc_4p0);
  DBG_PRINT(" mc10p0: "); 
  DBG_PRINT(spsData.mc_10p0);
  DBG_PRINT(" nc0p5: "); 
  DBG_PRINT(spsData.nc_0p5);
  DBG_PRINT(" nc1p0: "); 
  DBG_PRINT(spsData.nc_1p0);
  DBG_PRINT(" nc2p5: "); 
  DBG_PRINT(spsData.nc_2p5);
  DBG_PRINT(" nc4p0: "); 
  DBG_PRINT(spsData.nc_4p0);
  DBG_PRINT(" nc10p0: "); 
  DBG_PRINT(spsData.nc_10p0);
  DBG_PRINT(" partSize: "); 
  DBG_PRINTLN(spsData.typical_particle_size);

  DBG_PRINTLN(RTCGetString(rtc));
  DBG_PRINTLN();

  Serial.flush();
}