#include "communication.hpp"

void callback(char* topic, byte* message, unsigned int length)
{
  DBG_PRINTLN(F("Callback function"));
}

bool uploadData(PubSubClient& mqClient,DynamicJsonDocument &doc, char* topic)
{
  if(!doc.isNull())
  {
    DBG_PRINTLN("Doc not null");
    JsonArray arr = doc["logs"];
    for(auto e : arr)
    {
      String output;
      serializeJsonPretty(e, output);
      DBG_PRINTLN(output);
      if(!mqClient.publish(topic, output.c_str()))
        return false;
    }

    return true;
  }

  return false;
}

bool reportProblem(PubSubClient& mqClient, statusStruct& status, char* topic)
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