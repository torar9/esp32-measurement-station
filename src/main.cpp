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
double readBatteryLevel();

void setup() 
{
  delay(2000);
  DBG_SERIAL_BEGIN(BAUD_RATE);

  pinMode(BATTERY_PIN, INPUT);

  if(!cardPrepare(card, SD_CS))
  {
    DBG_PRINTLN("Unable to init SD card");
    cardAvailable = false;
  }

  if(!sps30Prepare())
    spsAvailable = false;
  else sps30_stop_measurement();

  if(!bme.begin())
  {
    DBG_PRINTLN("Unable to init BME680!");
    bmeAvailable = false;
  }

  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
      setupOTA();

      mqClient.setServer(mqtt_server, 1883);
      mqClient.setCallback(callback);
      mqClient.setBufferSize(MQTT_PACKET_SIZE);
  }

  if(!rtc.begin())
  {
    DBG_PRINTLN(F("Couldn't init RTC"));
    rtcAvailable = false;
  }

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

    if(!mqClient.connected())
    {
      DBG_PRINTLN(F("Recconecting MQTT client"));
      
      if(mqClient.connect(mqttID))
        goto upload;
    }
    else
    {
      upload:
        if(cardAvailable)
          cardLoadJSONFromFile(card, doc, (char*)FILE_NAME);
        addEventToJSON(doc, data);
        //serializeJsonPretty(doc, Serial);

        DBG_PRINTLN("Uploading data...");
        success = uploadData(doc);
        if(success && cardAvailable)
          cardClearFile(card, (char*)FILE_NAME);
    }
  }

  if(!success && cardAvailable)
  {
    DBG_PRINTLN("Failed to upload... saving data to SD card");
    if(cardAvailable)
      backupData(card, doc, data, (char*)FILE_NAME);
    //cardClearFile(card, (char*)FILE_NAME);
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
  DBG_PRINT("Battery level: ");
  DBG_PRINT(data.batteryLevel);
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
  if(bmeAvailable)
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
  data.time = (rtcAvailable)? RTCGetString(rtc) : (char*)"";
  sps30ReadNewData(data.spsData);
}

bool uploadData(DynamicJsonDocument &doc)
{
  if(!doc.isNull())
  {
    DBG_PRINTLN("Doc not null");
    JsonArray arr = doc["logs"];
    for(auto e : arr)
    {
      String output;
      serializeJsonPretty(e, output);
      DBG_PRINTLN(output);
      if(!mqClient.publish("esp32/jsonTest", output.c_str()))
        return false;
    }

    return true;
  }

  return false;
}

bool backupData(SDFS &card, DynamicJsonDocument &doc, measurments &data, char* filename)
{
  cardLoadJSONFromFile(card, doc, filename);
  DBG_PRINTLN("Printing doc:");

  addEventToJSON(doc, data);
  
  return cardWriteJSONToFile(card, doc, filename);
}

double readBatteryLevel()
{
  return map(analogRead(BATTERY_PIN), 0.0f, 4095.0f, 0, 100);
}