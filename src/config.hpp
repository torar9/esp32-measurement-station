#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "debug.hpp"

/** \file config.hpp
 * This file holds config values such as server ip, delay values, pin values etc.
 */

/**
 * Baud rate for serial communication.
 */
#define BAUD_RATE 115200
/**
 * Holds amount of uS in second.
 */
#define uS_TO_S_FACTOR (uint64_t)1000000
/**
 * Default amount of seconds for station to sleep.
 */
#define TIME_TO_SLEEP_DEFAULT (uint64_t)60
/**
 * Amount of seconds for station to sleep when battery level is high.
 */
#define TIME_TO_SLEEP_HIGH (uint64_t)60
/**
 * Amount of seconds for station to sleep when battery level is medium.
 */
#define TIME_TO_SLEEP_MEDIUM (uint64_t)1800
/**
 * Amount of seconds for station to sleep when battery level is low.
 */
#define TIME_TO_SLEEP_LOW (uint64_t)3600
/**
 * Loop delay.
 */
#define LOOP_DELAY 10000
/**
 * Type of DHT senzor.
 */
#define DHT_TYPE DHT22
/**
 * DHT pin value.
 */
#define DHT_PIN 17
/**
 * Build-in LED pin value.
 */
#define BUILD_IN_LED_PIN 2
/**
 * LED delay blink.
 */
#define LED_DELAY 300
/**
 * Battery analog pin value.
 */
#define BATTERY_PIN 32
/**
 * SD card CS pin.
 */
#define SD_CS 5
/**
 * JSON document buffer size in bytes for measured data.
 */
#define JSON_DOC_SIZE_MEASUREMENTS 12288
/**
 * JSON document buffer size in bytes for error report.
 */
#define JSON_DOC_SIZE_STATUS 192
/**
 * MQTT packet size in bytes.
 */
#define MQTT_PACKET_SIZE 512
/**
 * Full path file location for storing measured data.
 */
#define FILE_NAME "/station/station_data.json"
/**
 * Topic name for logs.
 */
#define LOG_TOPIC "esp32/log"
/**
 * Topic name for measured data.
 */
#define DATA_TOPIC "esp32/data"
/**
 * Topic name for error reports.
 */
#define REPORT_TOPIC "esp32/report"
/**
 * Quality of service (QoS) for MQTT publish communication.
 */
#define MQTT_PUB_QOS 2
/**
 * Quality of service (QoS) for MQTT subscribe communication.
 */
#define MQTT_SUB_QOS 1
/**
 * Sea level of current location of station.
 * Is used to calibrate sensors.
 */
#define SEA_LEVEL_PRESSURE 1034


static const char* ssid = ""; /**< Wi-Fi SSID. */
static const char* passwd = ""; /**< Wi-Fi password. */
static const char* ntpServer = "0.cz.pool.ntp.org"; /**< NTP server address. */
static const char* mqttID = "esp32"; /**< MQTT client ID. */
static const char* mqttName = "esp32"; /**< MQTT client name. */
static const char* mqttPasswd = ""; /**< MQTT client password. */
static const char* mqtt_server = ""; /**< MQTT server IP address. */
static const uint16_t mqtt_port = 1883; /**< MQTT server port. */
static const char* hostname = "esp32"; /**< Station hostname in local network. */
static const char* passwdHash = ""; /**< MQTT password hash. */
/**
 * GMT offset in seconds.
 * Is used to set time from NTP server.
 */
static const long gmtOffset_sec = 3600; /**< */
/**
 * UTC daylight offset.
 * Is used to set time from NTP server.
 */
static const int daylightOffset_sec = 3600;
static const int high_level = 70; /**< Define high level of battery. */
static const int medium_level = 40; /**< Define medium level of battery. */
static const int low_level = 20; /**< Define low level of battery */

#endif