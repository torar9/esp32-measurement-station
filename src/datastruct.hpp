#ifndef DATASTRUCT_HPP
#define DATASTRUCT_HPP

#include <sps30.hpp>

/** \file datastruct.hpp
 * This struct is used to hold measured data.
 */
struct measurements
{
  float humidity = NAN; /**< humidity Humidity level in % */
  char* time; /**< Time in "yyyy-MM-dd HH:mm:ss" format*/
  float batteryLevel = NAN; /**< Battery level in % */
  float temperature = NAN; /**< Temperature  in °C */
  float pressure = NAN; /**< Pressure in hpa */
  float altitude = NAN; /**< Altitude in meters */
  uint32_t gasResistance = 0; /**< Gas resistance */
  sps30_measurement spsData; /**< Contains data from sps30 sensor */
};

#endif