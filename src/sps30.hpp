#ifndef SPS30_HPP
#define SPS30_HPP

/** @cond */
#include <sps30.h>
/** @endcond */

/** \file sps30.hpp
 * Function for working with sps30 sensor.
 */

/**
 * Inicialize sps30 sensor.
 * @return Whatever or not initialization was successful.
 */
bool sps30Prepare();

/**
 * Read new data from sps30 sensor.
 * @param data sps30 data struct.
 * @return Whatever or not measurment was successful.
 */
bool sps30ReadNewData(sps30_measurement &data);

/**
 * Get string representation of serial number and firmware version of sps30 sensor.
 * @return string representation of serial number and firmware version of sps30 sensor.
 */
const char* sps30ModuleInfo();

#endif