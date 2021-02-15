#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "dataStruct.hpp"
#include "statstruct.hpp"

extern SDFS card;

bool cardPrepare();
bool cardWriteJSONToFile(DynamicJsonDocument &doc, char* fileName);
bool cardLoadJSONFromFile(DynamicJsonDocument &doc, char* fileName);
void addEventToJSON(DynamicJsonDocument &doc, measurments &event);
void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status);
bool cardClearFile(char* fileName);

#endif