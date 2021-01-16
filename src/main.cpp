#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_BME680.h>
#include "config.hpp"
#include "storage.hpp"
#include "sps30.hpp"
#include "rtcmodule.hpp"

WiFiClient wifiClient;
PubSubClient mqClient(wifiClient);
Adafruit_BME680 bmp;
RTC_DS3231 rtc;
SDFS card(SD);

void callback(char* topic, byte* message, unsigned int length);
void test();

void setup() 
{
  Serial.begin(9600);
  prepareSDCard(card, SD_CS);
  sps30Prepare();
  sps30_stop_measurement();
  //sps30_reset();
  //sps30_start_measurement();

  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
    setupOTA();

    mqClient.setServer(mqtt_server, 1883);
    mqClient.setCallback(callback);
  }

  setupRTCModule(rtc);

  if(rtc.lostPower() && WiFi.status() == WL_CONNECTED)
  {
    struct tm timeStr;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getLocalTime(&timeStr);

    setTimeOnline(rtc, timeStr);
  }

  //dhtSensor.begin();
    
  if(!bmp.begin())
  {
    Serial.println("Failed to initialize BMP");
    abort();
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
  float tmp = bmp.readTemperature();
  float press = bmp.readPressure();
  float alt = bmp.readAltitude(1013.25);
  float hum = bmp.readHumidity();
  sps30_measurement spsData;
  sps30ReadNewData(spsData);

  DBG_PRINT("temp: ");
  DBG_PRINT(tmp);
  DBG_PRINT(" press: ");
  DBG_PRINT(press / 100.0);
  DBG_PRINT(" alt: ");
  DBG_PRINT(alt);
  DBG_PRINT(" hum: ");
  DBG_PRINTLN(hum);
  DBG_PRINTLN(getRTCString(rtc));

  DBG_PRINT("mc1p0: ");
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


  DBG_PRINTLN();
  Serial.flush();
}