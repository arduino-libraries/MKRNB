/*
  Web ICMP Ping

  This sketch pings a device based on the IP address or the hostname
  using the NB-IoT module of Arduino MKR NB 1500 board. 

  created 8 Jenuary 2025
  by fabik111

 */

#include <MKRNB.h>
#include "arduino_secrets.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
const char APN[]           = "mobile.vodafone.it";
// initialize the library instance
GPRS gprs;
NB nbAccess;

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  while(nbAccess.begin(PINNUMBER, APN) != NB_READY){
    Serial.println("Not connected");
    delay(1000);
  }
  
  while(gprs.attachGPRS() != GPRS_READY){
    Serial.println("GPRS not attached");
    delay(1000);
  } 

  Serial.println("Connected!");
}

/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */

  // Ping IP
  const IPAddress remote_ip(140,82,121,4);
  Serial.print("Trying to ping github.com on IP: ");
  Serial.println(remote_ip);

  // using default ping count of 1
  int res = gprs.ping(remote_ip);

  if (res > 0) {
    Serial.print("Ping response time: ");
    Serial.print(res);
    Serial.println(" ms");
  }
  else {
    Serial.println("Timeout on IP!");
  }

  // Ping Host
  const char* remote_host = "www.google.com";
  Serial.print("Trying to ping host: ");
  Serial.println(remote_host);

  int res1 = gprs.ping(remote_host);

  if (res1 > 0) {
    Serial.print("Ping average response time: ");
    Serial.print(res1);
    Serial.println(" ms");
  }
  else {
    Serial.println("Timeout on host!");
  }

  Serial.println();
  delay(5000);
}

