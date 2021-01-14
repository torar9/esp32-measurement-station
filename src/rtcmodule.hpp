#ifndef RTCMODULE_HPP
#define RTCMODULE_HPP

#include <Arduino.h>
#include <time.h>
#include <string>
#include <RTClib.h>

void setupRTCModule(RTC_DS3231 &rtc);
void setTimeOnline(RTC_DS3231 &rtc, tm &timeStr);
const char* getRTCString(RTC_DS3231 &rtc);

#endif