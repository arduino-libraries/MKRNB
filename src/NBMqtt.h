

#ifndef _NB_MQTT_H_INCLUDED
#define _NB_MQTT_H_INCLUDED

#include "NB.h"

class NBMqtt
{

public:
  /** Constructor
      @param trace    if true, dumps all AT dialogue to Serial
      @return - 
  */
  NBMqtt(bool trace = false);

  /** Obtain MQTT Error
      @return MQTT Error
   */
  String getMQTTerror();

  /** Set MQTT ClientID
      @return -
   */
  String setMQTTClientID(char *mqttID);

  /** Set MQTT Port Number
      @return -
   */
  int setMQTTPort(int mqttPort);

  /** Set MQTT Username Password
      @return -
   */
  String setMQTTUserPassword(char *mqttUser, char *mqttPW);

  /** Set MQTT Broker URL
      @return -
   */
  String setMQTTBrokerURL(char *brokerURL);

  /** Set MQTT Broker IP
      @return -
   */
  String setMQTTBrokerIP(char *brokerIP);

  /** Connect / Disconnect MQTT Broker
      @return -
   */
  bool setMQTTBrokerConnect(bool con);

   /** Send a MQTT Message
      @return -
   */
  String sendMQTTMsg(char *mqttTopic, char *mqttMsg);

  /** Subscribe to a Topic
      @return -
   */
  String setMQTTSubscribe(char *sub_topic);
  
   /** MQTT NVram save and restore operations
      @return -
   */
  int setMQTTConfig(int setNVRam);

};

#endif
