#ifndef CONFIG_HPP
#define CONFIG_HPP

#define DEBUG 1
#if DEBUG
  #define DBG_SERIAL_SETUP(X) Serial.begin(X);
  #define DBG_PRINTF(X, Y) Serial.printf(X, Y);
  #define DBG_PRINT(x) Serial.print(x)
  #define DBG_PRINTDEC(x) Serial.print (x, DEC)
  #define DBG_PRINTLN(x) Serial.println (x)
#else
  #define DBG_SERIAL_SETUP(X)
  #define DBG_PRINTF(X, Y)
  #define DBG_PRINT(x)
  #define DBG_PRINTDEC(x)
  #define DBG_PRINTLN(x)
#endif

#endif