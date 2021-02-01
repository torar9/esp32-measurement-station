#ifndef SPS30_HPP
#define SPS30_HPP

#include <Arduino.h>
#include <sps30.h>

bool sps30Prepare();
bool sps30ReadNewData(sps30_measurement &data);
const char* sps30ModuleInfo();

#endif