#include "communication.hpp"

void callback(char* topic, byte* message, unsigned int length)
{
  log("Callback function activated");
}

bool uploadData(DynamicJsonDocument &doc, char* topic)
{
  if(!doc.isNull())
  {
    JsonArray arr = doc["logs"];
    for(auto e : arr)
    {
      String output;
      serializeJson(e, output);
      if(!mqClient.publish(topic, output.c_str()))
        return false;
    }

    return true;
  }

  return false;
}

bool reportProblem(statusStruct& status, char* topic)
{
  DynamicJsonDocument doc(JSON_DOC_SIZE_STATUS);

  addEventToJSON(doc, status);

  if(!doc.isNull())
  {
    String output;
    serializeJsonPretty(doc, output);

    bool result = mqClient.publish(topic, output.c_str());

    doc.clear();
    return result;
  }

  doc.clear();
  return false;
}

void log(const char* message, bool newLine, const char* topic)
{
  #if DEBUG
    if(mqClient.connected())
    mqClient.publish(topic, message);

    (newLine)? DBG_PRINTLN(message) : DBG_PRINT(message);
  #endif
}