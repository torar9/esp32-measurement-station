#include "storage.hpp"
#include "debug.hpp"
#include "communication.hpp"

bool cardPrepare()
{
    if(!SD.exists("/station"))
    {
        return SD.mkdir("/station");
    }
    return true;
}

bool cardWriteJSONToFile(DynamicJsonDocument &doc, char* fileName)
{
    File file = card.open(fileName, FILE_WRITE);
    if(!file)
    {
        log("Unable to open file for write!");

        return false;
    }

    serializeJson(doc, file);

    file.flush();
    file.close();

    return true;
}

bool cardLoadJSONFromFile(DynamicJsonDocument &doc, char* fileName)
{
    if(!card.exists(fileName))
        return false;

    File file = card.open(fileName, FILE_READ);
    if(!file)
    {
        log("Unable to open file for read!");

        return false;
    }

    deserializeJson(doc, file);

    file.flush();
    file.close();

    return true;
}

void addEventToJSON(DynamicJsonDocument &doc, measurments &event)
{
    JsonArray arr = doc["logs"].as<JsonArray>();
    if(arr.isNull())
    {
        arr = doc.createNestedArray("logs");
    }
    JsonObject log = arr.createNestedObject();
    
    log["altitude"] = event.altitude;
    log["humidity"] = event.humidity;
    log["pressure"] = event.pressure;
    log["time"] = event.time;
    log["temperature"] = event.temperature;
    log["batteryLevel"] = event.batteryLevel;
    log["mc_10p0"] = event.spsData.mc_10p0;
    log["mc_1p0"] = event.spsData.mc_1p0;
    log["mc_2p5"] = event.spsData.mc_2p5;
    log["mc_4p0"] = event.spsData.mc_4p0;
    log["nc_0p5"] = event.spsData.nc_0p5;
    log["nc_10p0"] = event.spsData.nc_10p0;
    log["nc_1p0"] = event.spsData.nc_1p0;
    log["nc_2p5"] = event.spsData.nc_2p5;
    log["nc_4p0"] = event.spsData.nc_4p0;
    log["typical_particle_size"] = event.spsData.typical_particle_size;
}

void addEventToJSON(DynamicJsonDocument &doc, statusStruct &status)
{
    doc["cardAvailable"] = status.cardAvailable;
    doc["bmeAvailable"] = status.bmeAvailable;
    doc["rtcAvailable"] = status.rtcAvailable;
    doc["spsAvailable"] = status.spsAvailable;
    doc["problemOccured"] = status.problemOccured;
}

bool cardClearFile(char* fileName)
{
    if(card.exists(fileName))
        return card.remove(fileName);
    else return false;
}