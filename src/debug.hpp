#ifndef DEBUG_HPP
#define DEBUG_HPP

/** \file debug.hpp
 * This file is for debug purposes.
 */

/**
 * Turns on or off debug logs in serial communication.
 */
#define DEBUG 1
#if DEBUG
  #define DBG_SERIAL_BEGIN(x) Serial.begin(x);
  #define DBG_SERIAL_SETUP(x) Serial.begin(x);
  #define DBG_PRINTF(x, y) Serial.printf(x, y);
  #define DBG_PRINT(x) Serial.print(x)
  #define DBG_PRINTDEC(x) Serial.print(x, DEC)
  #define DBG_PRINTLN(x) Serial.println(x)
  #define DBG_FLUSH() Serial.flush();
#else
  #define DBG_SERIAL_BEGIN(x)
  #define DBG_SERIAL_SETUP(X)
  #define DBG_PRINTF(X, Y)
  #define DBG_PRINT(x)
  #define DBG_PRINTDEC(x)
  #define DBG_PRINTLN(x)
  #define DBG_FLUSH()
#endif

#endif