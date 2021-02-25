/** @cond */
#include <Adafruit_BME680.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <Arduino.h>
#include <WiFi.h>
/** @endcond */
#include "communication.hpp"
#include "statstruct.hpp"
#include "jsonhelper.hpp"
#include "rtcmodule.hpp"
#include "storage.hpp"
#include "config.hpp"
#include "sps30.hpp"

/** \file main.cpp
 * Main file
 */

WiFiClient wifiClient; /**< Wi-Fi client */
PubSubClient mqClient(wifiClient); /**< MQTT client */
RTC_DS3231 rtc; /**< RTC module */
Adafruit_BME680 bme; /**< BME680 sensor */
SDFS card(SD); /**< SD card module */
statusStruct status; /**< Struct that contains availability status for each sensor or module */

/**
 * Is used to attempt to establish Wi-Fi connection
 */
void setupWifi();

/**
 * Get reading from all available sensors and modules.
 * @param data Data Struct in which measured data is stored
 * @param rtc rtc module
 * @param bme BME680 sensor
 */
void measure(measurements &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme);

/**
 * Read battery level
 * @return battery level in %
 */
double readBatteryLevel();

/**
 * Determine and set the time to put station to sleep
 * @param batteryLevel battery level in %
 * @return Sleep mode
 */
int setSleepTimer(float batteryLevel);


/**
 * Setup function
 * Is used to setup pin modes, MCU, and inicialize sensors and modules.
 * Establish Wi-Fi connection and connection to MQTT broker.
 */
void setup()
{
  setCpuFrequencyMhz(80);
  delay(2000);
  DBG_SERIAL_BEGIN(BAUD_RATE);

  btStop();
  setupWifi();
  if(WiFi.status() == WL_CONNECTED)
  {
    mqClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqClient.setCallback(callback);
    mqClient.setBufferSize(MQTT_PACKET_SIZE);
    mqClient.connect(MQTT_ID);
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
  else sps30_start_measurement();//sps30_stop_measurement();

  if(!bme.begin())
  {
    bmeError:
      log("Unable to init BME680!");
      status.bmeAvailable = false;
      status.problemOccured = true;
  }
  else
  {
    if(!bme.setTemperatureOversampling(BME680_OS_8X))
      goto bmeError;
    if(!bme.setHumidityOversampling(BME680_OS_2X))
      goto bmeError;
    if(!bme.setPressureOversampling(BME680_OS_4X))
      goto bmeError;
    if(!bme.setIIRFilterSize(BME680_FILTER_SIZE_3))
      goto bmeError;
    if(!bme.setGasHeater(320, 150))
      goto bmeError;
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

    configTime(gmtOffset_sec, daylightOffset_sec, NTP_SERVER);
    getLocalTime(&timeStr);

    RTCSetTimeOnline(timeStr);
  }

  esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_DEFAULT);

  //mqClient.subscribe((char*)"esp32/debug");
}

/**
 * Main loop function
 */
void loop() 
{
  bool success = false;
  DynamicJsonDocument doc(JSON_DOC_SIZE_MEASUREMENTS);
  measurements data;

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
      
      if(mqClient.connect(MQTT_ID))
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
  delay(5000);
  WiFi.disconnect();
  esp_deep_sleep_start();
}

void setupWifi()
{
  DBG_PRINTLN(F("Connecting to WiFi..."));
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SSID, WIFI_PASSWD);
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

void measure(measurements &data, RTC_DS3231 &rtc, Adafruit_BME680 &bme)
{
  if(status.bmeAvailable)
  {
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.altitude = bme.readAltitude(SEA_LEVEL_PRESSURE);
    data.pressure = bme.readPressure() / 100.0;
    data.gasResistance = bme.readGas();
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
  else if(batteryLevel >= HIGH_LEVEL)
  {
    log((char*)"Level sleep: 1");
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP_HIGH);

    return 1;
  }
  else if(batteryLevel < HIGH_LEVEL && batteryLevel >= MEDIUM_LEVEL)
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