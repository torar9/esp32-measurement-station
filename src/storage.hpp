#ifndef STORAGE_HPP
#define STORAGE_HPP

/** @cond */
#include <SD.h>
/** @endcond */
#include "statstruct.hpp"
#include "datastruct.hpp"

/** \file storage.hpp
 * Function for working with storage and JSON documents
 */

/** \var
 * Extern SDFS card
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
bool cardWriteJSONToFile(DynamicJsonDocument &doc, const char* fileName);

/**
 * Load measured data from file on SD card
 * @param doc JSON document into which measured data is stored
 * @param fileName Full path to file
 * @return Whatever or not operation was successful
 */
bool cardLoadJSONFromFile(DynamicJsonDocument &doc, char* fileName);

/**
 * Delete file on SD card
 * @param fileName Full path to file
 * @return Whatever or not operation was successful
 */
bool cardClearFile(const char* fileName);

#endif