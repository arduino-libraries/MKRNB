/*
  MQTT Client class extension for the MKRNB lib. Use the internal MQTT stack of the SARA R410 modem to do MQTT without a library, creating sockets etc. Easy & convenient!

  This sketch, for the MKR NB 1500 board, setup a MQTT connection and send a MQTT Message 
  Circuit:
   MKR NB 1500 board
   Antenna
   SIM card 
   
  created August 4th 2021
  Version 0.1 by Roman Bolt

Done:

Constructor: Example: NBMqtt mqtt;

getMQTTerror Get the last MQTT error from the modem.Example : mqtt.getMQTTerror();
setMQTTClientID  Set the MQTT client ID : Use your SIM IMEI. Example mqtt.setMQTTClientID("352753090834234");
setMQTTPort(Int): Set the MQTT Port (Normal is 1883, SSL is 8883(not yet implemented). Example :  mqtt.setMQTTPort(1883);
setMQTTUserPassword(String, String): Set Username / Password for the MQTT broker. Example: mqtt.setMQTTUserPassword("username", "password"); 
setMQTTbrokerURL(String): Set the MQTT broker URL (DNS is made automatic). Example: mqtt.setMQTTBrokerURL("public.cloud.shiftr.io"); 
setMQTTbrokerIP(String): Set the MQTT broker IP (not necessary in general, provide a broker URL!)
setMQTTBrokerConnect(bool): Connect / Disconnect to the MQTT broker. Example: mqtt.setMQTTBrokerConnect(true); to connect, mqtt.setMQTTBrokerConnect(false); to disconnect
sendMQTTMsg(String, String): Send a MQTT message to a topic. Example: mqtt.sendMQTTMsg("/TestTopic", "TestMessage");
setMQTTSubscribe(String): Subscribe to a topic. Example: setMQTTSubscribe("TestTopic");
setMQTTConfig(int): Save, restore and restore(factory settings) MQTT setting to/from the NVRam of the modem. Example: Save config to NVRam: mqtt.setMQTTConfig(2);, Load config from the NVRam mqtt.setMQTTConfig(2); Restore factory settings: mqtt.setMQTTConfig(0)

To do:

Read out MQTT messages
Set Timeouts
Set last will
SSL
  
*/

// include the NB library
#include <MKRNB.h>

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = "";

// initialize the library instances
NB nbAccess(true);
NBScanner scannerNetworks;
NBMqtt mqtt;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SARA MQTT Test");

  // connection state
  bool connected = false;

  // Start GSM connection
  
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");

  mqtt.setMQTTClientID("352753090834234"); // Set Client ID : USE your SIM's UMEI
  mqtt.setMQTTPort(1883);//Set mqtt Port
  mqtt.setMQTTBrokerURL("public.cloud.shiftr.io"); 
  mqtt.setMQTTUserPassword("public", "public"); // User, Password
  mqtt.setMQTTBrokerConnect(true);
  
}

void loop() {

mqtt.sendMQTTMsg("/TestTopic", "TestMessage");
// A little hint: If you want to construct the MQTT message out of Strings and other variables, use: 
// char* mqtt_msg = foo bar etc.
// mqtt.sendMQTTMsg("/TestTopic", mqtt_msg);  
  
String mqtt_error = mqtt.getMQTTerror();
Serial.print("MQTT Error (See page 408 of the u-blox AT Manual): ");
Serial.println(mqtt_error);

delay(5000);
Serial.println("Done");

}
