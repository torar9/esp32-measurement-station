#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "statstruct.hpp"
#include "datastruct.hpp"

/** \file storage.hpp
 * Function for working with storage and JSON documents
 */

/**
 * Extern SD card
 */
extern SDFS card;

/**
 * Inicialize and prepare SD card
 * @return Whatever or not initialization was successful
 */
bool cardPrepare();

/**
 * Store measured data into file on SD card
 * @param doc JSON document with measured data
 * @param fileName Full path to file
 * @return Whatever or not operation was successful
 */
bool cardWriteJSONToFile(DynamicJsonDocument &doc, char* fileName);

/**
 * Load measured data from file on SD card
 * @param doc JSON document into which measured data is stored
 * @param fileName Full path to file
 * @return Whatever or not operation was successful
 */
bool cardLoadJSONFromFile(DynamicJsonDocument &doc, char* fileName);

/**
 * Append measurements to JSON document
 * @param doc JSON document with data
 * @param event struct with measurement data
 */
void addEventToJSON(DynamicJsonDocument &doc, measurements &event);
/**
 * Append status to JSON document
 * @param doc JSON document with data
 * @param status struct with status data
 */
void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status);

/**
 * Delete file on SD card
 * @param fileName Full path to file
 * @return Whatever or not operation was successful
 */
bool cardClearFile(char* fileName);

#endif