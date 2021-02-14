#include "rtcmodule.hpp"
#include "debug.hpp"

#define BUFFER_SIZE 40

void RTCSetTimeOnline(tm &timeStr)
{
    //struct tm timeStr;

    //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    //getLocalTime(&timeStr);

    DateTime time = DateTime(timeStr.tm_year+1900, timeStr.tm_mon + 1, timeStr.tm_mday, timeStr.tm_hour, timeStr.tm_min, timeStr.tm_sec);
    rtc.adjust(time);
}

char* RTCGetString()
{
    static char buffer[BUFFER_SIZE];
    DateTime now = rtc.now();

    snprintf(buffer, BUFFER_SIZE, "%04d-%02d-%02d %02d:%02d:%02d", 
    now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

    return buffer;
}