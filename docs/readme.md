# MKRNB library

With the [Arduino MKR NB 1500](https://store.arduino.cc/products/arduino-mkr-nb-1500) and this library you can connect to the internet over a GSM network. The on board module operates in 4G, using LTE Cat M1 or NB1.

Arduino MKR NB 1500 has a modem that transfers data from a serial port to the GSM network. The modem executes operations via a series of AT commands. The library abstracts low level communications between the modem and SIM card. It relies on the Serial library for communication between the modem and Arduino.

Typically, each individual command is part of a larger series necessary to execute a particular function. The library can also receive information and return it to you when necessary.

To use this library

```
#include <MKRNB.h>
```

## Library structure

As the library enables multiple types of functionality, there are a number of different classes.

- The `NB` class takes care of commands to the radio modem. This handles the connectivity aspects of the module and registers your system in the 4G infrastructure. All of your GSM/GPRS programs will need to include an object of this class to handle the necessary low level communication.
- Send/receive SMS messages, managed by the `NB_SMS` class.
- The `GPRSClass` is for connecting to the internet.
- `NBClient` includes implementations for a client, similar to the Ethernet and WiFi libraries.
- A number of utility classes such as `NBScanner` and `NBModem`

## Library compatibility

The library tries to be as compatible as possible with the current Ethernet and WiFi101 library. Porting a program from an Arduino Ethernet or WiFi101 library to an Arduino with the MKR NB 1500 should be fairly easy. While it is not possible to simply run Ethernet or WiFi101 compatible code on the MKR NB 1500 as-is, some minor, library specific, modifications will be necessary, like including the GSM and GPRS specific libraries and getting network configuration settings from your cellular network provider.


## Examples

You can visit the [MKR NB 1500 documentation portal](https://docs.arduino.cc/hardware/mkr-nb-1500) for tutorials, technical specifications and more. You can also browse through a set of examples at the [Examples from Libraries page](https://docs.arduino.cc/library-examples/#wifi-library).