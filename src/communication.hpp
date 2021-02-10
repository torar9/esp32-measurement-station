#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "storage.hpp"
#include "statstruct.hpp"
#include "debug.hpp"
#include "config.hpp"

void callback(char* topic, byte* message, unsigned int length);
bool uploadData(PubSubClient& mqClient, DynamicJsonDocument &doc, char* topic);
bool reportProblem(PubSubClient& mqClient, statusStruct& status, char* topic);

#endif