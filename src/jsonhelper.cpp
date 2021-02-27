#include "jsonhelper.hpp"

void addEventToJSON(DynamicJsonDocument &doc, measurements &event)
{
    doc[F("altitude")] = event.altitude;
    doc[F("humidity")] = event.humidity;
    doc[F("pressure")] = event.pressure;
    doc[F("time")] = event.time;
    doc[F("gasResistance")] = event.gasResistance;
    doc[F("temperature")] = event.temperature;
    doc[F("batteryLevel")] = event.batteryLevel;
    doc[F("mc10p0")] = event.spsData.mc_10p0;
    doc[F("mc1p0")] = event.spsData.mc_1p0;
    doc[F("mc2p5")] = event.spsData.mc_2p5;
    doc[F("mc4p0")] = event.spsData.mc_4p0;
    doc[F("nc0p5")] = event.spsData.nc_0p5;
    doc[F("nc10p0")] = event.spsData.nc_10p0;
    doc[F("nc1p0")] = event.spsData.nc_1p0;
    doc[F("nc2p5")] = event.spsData.nc_2p5;
    doc[F("nc4p0")] = event.spsData.nc_4p0;
    doc[F("typicalParticleSize")] = event.spsData.typical_particle_size;
}

void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status)
{
    doc[F("cardAvailable")] = status.cardAvailable;
    doc[F("bmeAvailable")] = status.bmeAvailable;
    doc[F("rtcAvailable")] = status.rtcAvailable;
    doc[F("spsAvailable")] = status.spsAvailable;
    doc[F("problemOccured")] = status.problemOccured;
}