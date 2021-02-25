#include "jsonhelper.hpp"

void addEventToJSON(DynamicJsonDocument &doc, measurements &event)
{
    JsonArray arr = doc["logs"].as<JsonArray>();
    if(arr.isNull())
    {
        arr = doc.createNestedArray("logs");
    }
    JsonObject log = arr.createNestedObject();
    
    log[F("altitude")] = event.altitude;
    log[F("humidity")] = event.humidity;
    log[F("pressure")] = event.pressure;
    log[F("time")] = event.time;
    log[F("gasResistance")] = event.gasResistance;
    log[F("temperature")] = event.temperature;
    log[F("batteryLevel")] = event.batteryLevel;
    log[F("mc10p0")] = event.spsData.mc_10p0;
    log[F("mc1p0")] = event.spsData.mc_1p0;
    log[F("mc2p5")] = event.spsData.mc_2p5;
    log[F("mc4p0")] = event.spsData.mc_4p0;
    log[F("nc0p5")] = event.spsData.nc_0p5;
    log[F("nc10p0")] = event.spsData.nc_10p0;
    log[F("nc1p0")] = event.spsData.nc_1p0;
    log[F("nc2p5")] = event.spsData.nc_2p5;
    log[F("nc4p0")] = event.spsData.nc_4p0;
    log[F("typicalParticleSize")] = event.spsData.typical_particle_size;
}

void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status)
{
    doc[F("cardAvailable")] = status.cardAvailable;
    doc[F("bmeAvailable")] = status.bmeAvailable;
    doc[F("rtcAvailable")] = status.rtcAvailable;
    doc[F("spsAvailable")] = status.spsAvailable;
    doc[F("problemOccured")] = status.problemOccured;
}