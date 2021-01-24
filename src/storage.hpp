#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "dataStruct.hpp"

bool cardPrepare(SDFS &card, uint8_t pin);
bool cardWriteJSONToFile(SDFS &card, DynamicJsonDocument &doc, char* fileName);
bool cardLoadJSONFromFile(SDFS &card, DynamicJsonDocument &doc, char* fileName);
void addEventToJSON(DynamicJsonDocument &doc, measurments &event);
bool cardClearFile(SDFS &card, char* fileName);

#endif