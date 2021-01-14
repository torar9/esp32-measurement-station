#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define DEBUG 1
#if DEBUG
  #define DBG_SERIAL_SETUP(X) Serial.begin(X);
  #define DBG_PRINTF(X, Y) Serial.printf(X, Y);
  #define DBG_PRINT(x) Serial.print(x)
  #define DBG_PRINTDEC(x) Serial.print (x, DEC)
  #define DBG_PRINTLN(x) Serial.println (x)
#else
  #define DBG_SERIAL_SETUP(X)
  #define DBG_PRINTF(X, Y)
  #define DBG_PRINT(x)
  #define DBG_PRINTDEC(x)
  #define DBG_PRINTLN(x)
#endif

#define BAUD_RATE 115200
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 10
#define LOOP_DELAY 10000
#define DHT_TYPE DHT22
#define DHT_PIN 17
#define BUILD_IN_LED_PIN 2
#define LED_DELAY 300
#define BATTERY_PIN 32
#define SD_CS 5

const char* ssid = "Damian";
const char* passwd = "";
const char* ntpServer = "0.cz.pool.ntp.org";
const char* mqttID = "esp32";
const char* mqttName = "ghost";
const char* mqttPasswd = "";
const char* mqtt_server = "192.168.0.38";
const char* hostname = "ESP-32";
const char* passwdHash = "";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void setupWifi()
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

void setupOTA()
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
      Serial.println("Start updating " + type);
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
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

#endif