#ifndef DATASTRUCT_HPP
#define DATASTRUCT_HPP

#include <sps30.hpp>

struct measurments
{
  float humidity = NAN;
  char* time;
  float batteryLevel = NAN;
  float temperature = NAN;
  float pressure = NAN;
  float altitude = NAN;
  uint32_t gasResistance = 0;
  sps30_measurement spsData;
};

#endif