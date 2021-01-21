#ifndef DATASTRUCT_HPP
#define DATASTRUCT_HPP

#include <sps30.hpp>

struct measurments
{
  int humidity;
  char* time;
  double batteryLevel;
  double temperature;
  double pressure;
  double altitude;
  sps30_measurement spsData;
};

#endif