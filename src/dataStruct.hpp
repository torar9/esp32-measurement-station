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
  sps30_measurement spsData;
};

#endif