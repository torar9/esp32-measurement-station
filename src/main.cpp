#include <Adafruit_BME680.h>
#include <PubSubClient.h>
#include <ArduinoJson.hpp>
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

void setupWifi();
void setupOTA();
void measure(measurments &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme);
double readBatteryLevel();
int setSleepTimer(float batteryLevel);

void setup() 
{
  setCpuFrequencyMhz(80);
  delay(2000);
  DBG_SERIAL_BEGIN(BAUD_RATE);

  btStop();
  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
    //ArduinoOTA.setHostname(hostname);
    //setupOTA();
    //ArduinoOTA.begin();

    mqClient.setServer(mqtt_server, mqtt_port);
    mqClient.setCallback(callback);
    mqClient.setBufferSize(MQTT_PACKET_SIZE);
    mqClient.connect(mqttID);
  }

  pinMode(BATTERY_PIN, INPUT);

  if(!card.begin(SD_CS))
  {
    if(!card.begin(SD_CS))
    {
      log("Unable to init SD card");
      status.cardAvailable = false;
      status.problemOccured = true;
    }
  }
  else
  {
    if(!cardPrepare())
    {
      log("Unable to create folder");
      status.cardAvailable = false;
      status.problemOccured = true;
    }
  }

  if(!sps30Prepare())
  {
    log("could not probe / connect with SPS30.");
    status.spsAvailable = false;
    status.problemOccured = true;
  }
  else sps30_stop_measurement(); //sps30_start_measurement();//

  if(!bme.begin())
  {
    log("Unable to init BME680!");
    status.bmeAvailable = false;
    status.problemOccured = true;
  }

  if(!rtc.begin())
  {
    log("Couldn't init RTC");
    status.rtcAvailable = false;
    status.problemOccured = true;
  }

  if(rtc.lostPower() && WiFi.status() == WL_CONNECTED)
  {
    struct tm timeStr;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getLocalTime(&timeStr);

    RTCSetTimeOnline(timeStr);
  }

  esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_DEFAULT);

  //mqClient.subscribe((char*)"esp32/debug");
}

void loop() 
{
  bool success = false;
  DynamicJsonDocument doc(JSON_DOC_SIZE_MEASUREMENTS);
  measurments data;

  log("Main loop...");
  log("Starting to measure...");
  measure(data, rtc, bme);

  if(status.cardAvailable && card.exists((char*)FILE_NAME))
  {
    log("Trying to load data from file...");
    cardLoadJSONFromFile(doc, (char*)FILE_NAME);
  }
  serializeJsonPretty(doc, Serial);
  addEventToJSON(doc, data);
  serializeJsonPretty(doc, Serial);

  if(WiFi.status() == WL_CONNECTED)
  {
    if(!mqClient.connected())
    {
      log("Recconecting MQTT client");
      
      if(mqClient.connect(mqttID))
        goto upload;
    }
    else
    {
      upload:
        log("Uploading data...");
        success = uploadData(doc, (char*)DATA_TOPIC);

        if(status.problemOccured)
          reportProblem(status, (char*)REPORT_TOPIC);
    }
  }

  if(!success && status.cardAvailable)
  {
    log("Failed to upload... saving data to SD card");
    cardWriteJSONToFile(doc, (char*)FILE_NAME);
  }

  if(success && status.cardAvailable)
    cardClearFile((char*)FILE_NAME);
  
  doc.clear();

  setSleepTimer(data.batteryLevel);
  sps30_stop_measurement();
  log("End of main loop...");
  DBG_FLUSH();
  //ArduinoOTA.handle();
  delay(5000);
  WiFi.disconnect();
  esp_deep_sleep_start();
}

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
  data.time = (status.rtcAvailable)? RTCGetString() : (char*)"";
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
    log("Level sleep: 0");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_DEFAULT);

    return 0;
  }
  else if(batteryLevel >= high_level)
  {
    log((char*)"Level sleep: 1");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_HIGH);

    return 1;
  }
  else if(batteryLevel < high_level && batteryLevel >= medium_level)
  {
    log("Level sleep: 2");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_MEDIUM);

    return 2;
  }
  else 
  {
    log("Level sleep: 3");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_LOW);

    return 3;
  }
}