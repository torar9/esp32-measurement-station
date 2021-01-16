#ifndef RTCMODULE_HPP
#define RTCMODULE_HPP

#include <Arduino.h>
#include <time.h>
#include <string>
#include <RTClib.h>

void RTCPrepare(RTC_DS3231 &rtc);
void RTCSetTimeOnline(RTC_DS3231 &rtc, tm &timeStr);
const char* RTCGetString(RTC_DS3231 &rtc);

#endif