/*
TO DO
AT+UMQTT 0
AT+UMQTT 1
AT+UMQTT 2

AT+UMQTT 4
AT+UMQTTC=1
AT+UMQTTC=2


*/
#include "Modem.h"

#include "NBMqtt.h"

NBMqtt::NBMqtt(bool trace)
{
  if (trace)
  {
    MODEM.debug();
  }
}

String NBMqtt::getMQTTerror()
{
  String response;

  MODEM.send("AT+UMQTTER");
  if (MODEM.waitForResponse(1000, &response) == 1)
  {
    int firstSpaceIndex = response.indexOf(' ');
    int lastCommaIndex = response.lastIndexOf(',');

    if (firstSpaceIndex != -1 && lastCommaIndex != -1)
    {
      return response.substring(firstSpaceIndex + 1, lastCommaIndex);
    }
  }

  return "";
}

String NBMqtt::setMQTTClientID(char *mqttID)
{
  MODEM.sendf("AT+UMQTT=0,\"%s\"", mqttID);
  MODEM.waitForResponse(10000);
  return "";
}

int NBMqtt::setMQTTPort(int mqttPort)
{
  MODEM.sendf("AT+UMQTT=1,%d", mqttPort);
  MODEM.waitForResponse(10000);
  return 0;
}

String NBMqtt::setMQTTUserPassword(char *mqttUser, char *mqttPW)
{
  MODEM.sendf("AT+UMQTT=4,\"%s\",\"%s\"", mqttUser, mqttPW);
  MODEM.waitForResponse(10000);
  return "";
}

String NBMqtt::setMQTTBrokerURL(char *brokerURL)
{
  MODEM.sendf("AT+UMQTT=2,\"%s\"", brokerURL);
  MODEM.waitForResponse(10000);
  return "";
}

String NBMqtt::setMQTTBrokerIP(char *brokerIP)
{
  MODEM.sendf("AT+UMQTT=3,\"%s\"", brokerIP);
  MODEM.waitForResponse(10000);
  return "";
}

bool NBMqtt::setMQTTBrokerConnect(bool con)
{
  MODEM.sendf("AT+UMQTTC=%d\n", con);
  MODEM.waitForResponse(10000);
  return "";
}

String NBMqtt::sendMQTTMsg(char *mqttTopic, char *mqttMsg)
{
  MODEM.sendf("AT+UMQTT=2,0,0,\"%s\",\"%s\"", mqttTopic, mqttMsg);
  MODEM.waitForResponse(10000);
  return "";
}

String NBMqtt::setMQTTSubscribe(char *sub_topic)
{
  MODEM.sendf("AT+UMQTTC=4,0,\"%s\"", sub_topic);
  MODEM.waitForResponse(10000);
  return "";
}
