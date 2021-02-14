#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "debug.hpp"

#define BAUD_RATE 115200
#define uS_TO_S_FACTOR (uint64_t)1000000
#define TIME_TO_SLEEP_DEFAULT (uint64_t)60
#define TIME_TO_SLEEP_HIGH (uint64_t)60
#define TIME_TO_SLEEP_MEDIUM (uint64_t)1800
#define TIME_TO_SLEEP_LOW (uint64_t)3600
#define LOOP_DELAY 10000
#define DHT_TYPE DHT22
#define DHT_PIN 17
#define BUILD_IN_LED_PIN 2
#define LED_DELAY 300
#define BATTERY_PIN 32
#define SD_CS 5
#define JSON_DOC_SIZE_MEASUREMENTS 12288
#define JSON_DOC_SIZE_STATUS 192
#define MQTT_PACKET_SIZE 512
#define FILE_NAME "/station/station_data.json"
#define LOG_TOPIC "esp32/log"
#define DATA_TOPIC "esp32/data"
#define REPORT_TOPIC "esp32/report"

static const char* ssid = "";
static const char* passwd = "";
static const char* ntpServer = "0.cz.pool.ntp.org";
static const char* mqttID = "esp32";
static const char* mqttName = "esp32";
static const char* mqttPasswd = "";
static const char* mqtt_server = "";
static const uint16_t mqtt_port = 1883;
static const char* hostname = "esp32";
static const char* passwdHash = "";
static const long gmtOffset_sec = 3600;
static const int daylightOffset_sec = 3600;
static const int high_level = 70;
static const int medium_level = 40;
static const int low_level = 20;

inline void setupWifi()
{
  DBG_PRINTLN(F("Connecting to WiFi..."));
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setHostname(hostname);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, passwd);
  WiFi.waitForConnectResult();

  if(WiFi.status() != WL_CONNECTED)
  {
    DBG_PRINTLN(F("Unable to connect to wifi"));
  }
  else
  {
    DBG_PRINTLN(F("Connected to WiFi."));
    DBG_PRINT(F("IPv4 address: "));
    DBG_PRINTLN(WiFi.localIP());
  }
}

inline void setupOTA()
{
  ArduinoOTA
    .onStart([]()
    {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      DBG_PRINTLN("Start updating " + type);
    })
    .onEnd([]()
    {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total)
    {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error)
    {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) DBG_PRINTLN(F("Auth Failed"));
      else if (error == OTA_BEGIN_ERROR) DBG_PRINTLN(F("Begin Failed"));
      else if (error == OTA_CONNECT_ERROR) DBG_PRINTLN(F("Connect Failed"));
      else if (error == OTA_RECEIVE_ERROR) DBG_PRINTLN(F("Receive Failed"));
      else if (error == OTA_END_ERROR) DBG_PRINTLN(F("End Failed"));
    });

  ArduinoOTA.begin();
}

#endif