#include <stdio.h>
#include <string.h>
#include "communication.hpp"
#include "jsonhelper.hpp"

void callback(String &topic, String &payload)
{
  log("Callback function activated");
}

bool uploadData(DynamicJsonDocument &doc, char* topic)
{
  if(!doc.isNull())
  {
    String output;
    char buffer[MQTT_PACKET_SIZE];
    size_t n = serializeJson(doc, buffer);

    return mqttClient.publish(topic, buffer, n, MQTT_PUB_QOS);
  }

  return false;
}

void log(const char* message, bool newLine, const char* topic)
{
  #if DEBUG
    if(mqttClient.connected())
    {
      if(!mqttClient.publish(topic, message, (unsigned)strlen(message), MQTT_PUB_QOS))
        DBG_PRINTLN("Failed to send log message...");
    }

    (newLine)? DBG_PRINTLN(message) : DBG_PRINT(message);
  #endif
}