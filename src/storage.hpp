#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <PubSubClient.h>
#include "dataStruct.hpp"
#include "statstruct.hpp"

bool cardPrepare(SDFS &card);
bool cardWriteJSONToFile(SDFS &card, DynamicJsonDocument &doc, char* fileName);
bool cardLoadJSONFromFile(SDFS &card, DynamicJsonDocument &doc, char* fileName);
void addEventToJSON(DynamicJsonDocument &doc, measurments &event);
void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status);
bool cardClearFile(SDFS &card, char* fileName);
bool cardBackupData(SDFS &card, DynamicJsonDocument &doc, measurments &data, char* filename);

#endif