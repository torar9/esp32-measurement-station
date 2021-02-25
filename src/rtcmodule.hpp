#ifndef RTCMODULE_HPP
#define RTCMODULE_HPP

/** @cond */
#include <RTClib.h>
/** @endcond */

/** \file rtcmodule.hpp
 * Function for working with RTC module
 */

/** \var
 * Extern RTC module
 */
extern RTC_DS3231 rtc;

/**
 * Set time in RTC module from BTP server.
 * @param timeStr time struct
 */
void RTCSetTimeOnline(tm &timeStr);

/**
 * Get string representation of time in "yyyy-MM-dd HH:mm:ss" format.
 * @return string representation of time
 */
char* RTCGetString();

#endif