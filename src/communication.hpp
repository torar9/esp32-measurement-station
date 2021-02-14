#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <string>
#include "storage.hpp"
#include "statstruct.hpp"
#include "debug.hpp"
#include "config.hpp"

extern PubSubClient mqClient;

void callback(char* topic, byte* message, unsigned int length);
bool uploadData(DynamicJsonDocument &doc, char* topic);
bool reportProblem(statusStruct& status, char* topic);
void log(const char* message, bool newLine = true, const char* topic = LOG_TOPIC);

#endif