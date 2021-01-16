#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

void CardPrepare(SDFS &card, uint8_t pin);

#endif