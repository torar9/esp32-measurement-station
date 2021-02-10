#include <Adafruit_BME680.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "communication.hpp"
#include "dataStruct.hpp"
#include "statstruct.hpp"
#include "rtcmodule.hpp"
#include "storage.hpp"
#include "config.hpp"
#include "debug.hpp"
#include "sps30.hpp"

WiFiClient wifiClient;
PubSubClient mqClient(wifiClient);
RTC_DS3231 rtc;
Adafruit_BME680 bme;
SDFS card(SD);
statusStruct status;

void measure(measurments &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme);
double readBatteryLevel();
int setSleepTimer(float batteryLevel);

void setup() 
{
  delay(2000);
  DBG_SERIAL_BEGIN(BAUD_RATE);

  pinMode(BATTERY_PIN, INPUT);

  if(!card.begin(SD_CS))
  {
    if(!card.begin(SD_CS))
    {
      DBG_PRINTLN("Unable to init SD card");
      status.cardAvailable = false;
      status.problemOccured = true;
    }
  }
  else
  {
    if(!cardPrepare(card))
    {
      DBG_PRINTLN("Unable to create folder");
      status.cardAvailable = false;
      status.problemOccured = true;
    }
  }

  if(!sps30Prepare())
  {
    status.spsAvailable = false;
    status.problemOccured = true;
  }
  else sps30_stop_measurement();

  if(!bme.begin())
  {
    DBG_PRINTLN("Unable to init BME680!");
    status.bmeAvailable = false;
    status.problemOccured = true;
  }

  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
    setupOTA();

    mqClient.setServer(mqtt_server, mqtt_port);
    mqClient.setCallback(callback);
    mqClient.setBufferSize(MQTT_PACKET_SIZE);
  }

  if(!rtc.begin())
  {
    DBG_PRINTLN(F("Couldn't init RTC"));
    status.rtcAvailable = false;
    status.problemOccured = true;
  }

  if(rtc.lostPower() && WiFi.status() == WL_CONNECTED)
  {
    struct tm timeStr;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getLocalTime(&timeStr);

    RTCSetTimeOnline(rtc, timeStr);
  }

  esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_DEFAULT);
}

void loop() 
{
  DBG_PRINTLN("Start...");
  bool success = false;
  DynamicJsonDocument doc(JSON_DOC_SIZE_MEASUREMENTS);
  measurments data;

  measure(data, rtc, bme);

  if(WiFi.status() == WL_CONNECTED)
  {
    DBG_PRINTLN("Wi-Fi connected.... uploading data");

    if(!mqClient.connected())
    {
      DBG_PRINTLN(F("Recconecting MQTT client"));
      
      if(mqClient.connect(mqttID))
        goto upload;
    }
    else
    {
      upload:
        if(status.cardAvailable)
          cardLoadJSONFromFile(card, doc, (char*)FILE_NAME);
        
        addEventToJSON(doc, data);
        //serializeJsonPretty(doc, Serial);

        DBG_PRINTLN("Uploading data...");
        success = uploadData(mqClient, doc, (char*)"esp32/jsonTest");

        //reportProblem(mqClient, status, (char*)"esp32/jsonStatus");
        if(status.problemOccured)
          reportProblem(mqClient, status, (char*)"esp32/jsonStatus");

        if(success && status.cardAvailable)
          cardClearFile(card, (char*)FILE_NAME);
    }
  }

  if(!success && status.cardAvailable)
  {
    DBG_PRINTLN("Failed to upload... saving data to SD card");
    if(status.cardAvailable)
      cardBackupData(card, doc, data, (char*)FILE_NAME);
    //cardClearFile(card, (char*)FILE_NAME);
  }
  
  doc.clear();
  DBG_PRINTLN("End...");
  DBG_FLUSH();

  setSleepTimer(data.batteryLevel);
  esp_deep_sleep_start();
}

void measure(measurments &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme)
{
  if(status.bmeAvailable)
  {
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.altitude = bme.readAltitude(1013.25);
    data.pressure = bme.readPressure() / 100.0;
  }
  else
  {
    data.temperature = NAN;
    data.humidity = NAN;
    data.altitude = NAN;
    data.pressure = NAN;
  }
  data.batteryLevel = readBatteryLevel();
  data.time = (status.rtcAvailable)? RTCGetString(rtc) : (char*)"";
  sps30ReadNewData(data.spsData);
}

double readBatteryLevel()
{
  return map(analogRead(BATTERY_PIN), 0.0f, 4095.0f, 0, 100);
}

int setSleepTimer(float batteryLevel)
{
  if(batteryLevel == 0 || batteryLevel == NAN)
  {
    DBG_PRINTLN("Level sleep: 0");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_DEFAULT);

    return 0;
  }
  else if(batteryLevel >= high_level)
  {
    DBG_PRINTLN("Level sleep: 1");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_HIGH);

    return 1;
  }
  else if(batteryLevel < high_level && batteryLevel >= medium_level)
  {
    DBG_PRINTLN("Level sleep: 2");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_MEDIUM);

    return 2;
  }
  else 
  {
    DBG_PRINTLN("Level sleep: 3");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_LOW);

    return 3;
  }
}