#ifndef SPS30_HPP
#define SPS30_HPP

#include <Arduino.h>
#include <sps30.h>

void prepareSPS30();
bool readNewData(sps30_measurement &data);
const char* toStringModuleInfo();

#endif