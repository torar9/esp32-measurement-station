#ifndef RTCMODULE_HPP
#define RTCMODULE_HPP

#include <Arduino.h>
#include <time.h>
#include <RTClib.h>

extern RTC_DS3231 rtc;

void RTCSetTimeOnline(tm &timeStr);
char* RTCGetString();

#endif