#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "config.hpp"
#include "storage.hpp"
#include "sps30.hpp"
#include "rtcmodule.hpp"

WiFiClient wifiClient;
PubSubClient mqClient(wifiClient);
DHT dhtSensor(DHT_PIN, DHT_TYPE);
RTC_DS3231 rtc;
SDFS card(SD);

void callback(char* topic, byte* message, unsigned int length);
void test();

void setup() 
{
    Serial.begin(9600);
    prepareSDCard(card, SD_CS);
    prepareSPS30();

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

    dhtSensor.begin();

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
    delay(1000);
    dhtSensor.read();
    DBG_PRINT("temp: ");
    DBG_PRINT(dhtSensor.readTemperature());
    DBG_PRINT(" humidity: ");
    DBG_PRINT(dhtSensor.readHumidity());
    DBG_PRINTLN(" %");

    DBG_PRINTLN();

    DBG_PRINTLN(getRTCString(rtc));
    Serial.flush();
}