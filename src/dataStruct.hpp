#ifndef DATASTRUCT_HPP
#define DATASTRUCT_HPP

#include <sps30.hpp>

struct measurments
{
  float humidity;
  char* time;
  float batteryLevel;
  float temperature;
  float pressure;
  float altitude;
  sps30_measurement spsData;
};

#endif