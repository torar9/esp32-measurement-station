#include <ArduinoJson.h>
#include "storage.hpp"
#include "communication.hpp"

bool cardPrepare()
{
    if(!SD.exists("/station"))
    {
        return SD.mkdir("/station");
    }
    return true;
}

bool cardWriteJSONToFile(DynamicJsonDocument &doc, const char* fileName)
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

bool cardClearFile(const char* fileName)
{
    if(card.exists(fileName))
        return card.remove(fileName);
    else return false;
}