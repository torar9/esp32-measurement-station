#include <Adafruit_BME680.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "dataStruct.hpp"
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

void callback(char* topic, byte* message, unsigned int length);
void test(measurments &data);
void measure(measurments &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme);
bool uploadData(DynamicJsonDocument &doc);
bool backupData(SDFS &card, DynamicJsonDocument &doc, measurments &data, char* filename);

void setup() 
{
  delay(2000);
  DBG_SERIAL_BEGIN(BAUD_RATE);
  cardPrepare(card, SD_CS);
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
  bool success = false;
  DynamicJsonDocument doc(JSON_DOC_SIZE);
  measurments data;

  measure(data, rtc, bme);

  if(WiFi.status() == WL_CONNECTED)
  {
    DBG_PRINTLN("Wi-Fi connected.... uploading data");

    if(mqClient.state() != MQTT_CONNECTED)
    {
      DBG_PRINTLN(F("Recconecting MQTT client"));
      
      if(mqClient.connect(mqttID, mqttName, mqttPasswd))
      {
        cardLoadJSONFromFile(card, doc, (char*)FILE_NAME);
        addEventToJSON(doc, data);
        test(data);
        
        success = uploadData(doc);
      }
    }
    else
    {
      cardLoadJSONFromFile(card, doc, (char*)FILE_NAME);
      addEventToJSON(doc, data);
      test(data);

      success = uploadData(doc);
    }
  }

  if(!success)
  {
    DBG_PRINTLN("Failed to upload... saving data to SD card");
    backupData(card, doc, data, (char*)FILE_NAME);
  }
  
  doc.clear();
  DBG_FLUSH();
  esp_light_sleep_start();
}

void callback(char* topic, byte* message, unsigned int length)
{
  DBG_PRINTLN(F("Callback function"));
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
}

void measure(measurments &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme)
{
  data.temperature = bme.readTemperature();
  data.humidity = bme.readHumidity();
  data.altitude = bme.readAltitude(1013.25);
  data.pressure = bme.readPressure() / 100.0;
  data.time = RTCGetString(rtc);
  sps30ReadNewData(data.spsData);
}

bool uploadData(DynamicJsonDocument &doc)
{
  DBG_PRINTLN("Uploading data...");

  return true;
}

bool backupData(SDFS &card, DynamicJsonDocument &doc, measurments &data, char* filename)
{
  cardLoadJSONFromFile(card, doc, filename);
  DBG_PRINTLN("Printing doc:");
  serializeJsonPretty(doc, Serial);

  DBG_PRINTLN("Printing data from struct:");
  test(data);

  addEventToJSON(doc, data);
  
  return cardWriteJSONToFile(card, doc, filename);
}