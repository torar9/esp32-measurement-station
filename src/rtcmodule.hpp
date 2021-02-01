#ifndef RTCMODULE_HPP
#define RTCMODULE_HPP

#include <Arduino.h>
#include <time.h>
#include <string>
#include <RTClib.h>

void RTCSetTimeOnline(RTC_DS3231 &rtc, tm &timeStr);
char* RTCGetString(RTC_DS3231 &rtc);

#endif