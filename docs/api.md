# MKRNB Library

## NB Class

### `NB constructor`

#### Description

NB is the base class for all NB based functions.

#### Syntax

```
NB nbAccess
NB nbAccess(debug)
```

#### Parameters
debug: boolean (default FALSE) flag for turning on the debug mode. This will print out the AT commands from the modem.

#### Example

```

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NB nbAccess;     // include a 'true' parameter for debug enabled

void setup()
{
  // initialize serial communications
  Serial.begin(9600);

  // connection state
  boolean notConnected = true;

  // Start NB Module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==NB_READY){
      notConnected = false;
      Serial.println("Connected to network");
    }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}

void loop()
{
  // Nothing here
}
 
```

### `begin()`

#### Description

Connects to the NB network identified on the SIM card.

#### Syntax

```
nb.begin()
nb.begin(pin)
nb.begin(pin, restart)
nb.begin(pin, restart, sync)
```

#### Parameters

- pin : character array with the PIN to access a SIM card (default = 0)
- restart : boolean, determines whether to restart modem or not (default= true)
- sync : boolean, synchronous (true, default) or asynchronous (false) mode

#### Returns
char : 0 if asynchronous. If synchronous, returns status : ERROR, IDLE, CONNECTING, NB_READY, GPRS_READY, TRANSPARENT_CONNECTED

#### Example

```
#include <MKRNB.h>

#define PINNUMBER ""

NB nb; // include a 'true' parameter for debug enabled

void setup()
{
  // initialize serial communications
  Serial.begin(9600);

  // connection state
  boolean notConnected = true;

  // Start NB Module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(nb.begin(PINNUMBER)==NB_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop()
{
// once connected do something interesting
}
```

### `shutdown()`

#### Description

Disconnects from the NB network identified on the SIM card by powering the modem off.

#### Syntax

```
nb.shutdown()

```

#### Parameters
none

#### Returns
True if successful

#### Example

```
#include <MKRNB.h>

#define PINNUMBER ""

NB nb; // include a 'true' parameter for debug enabled

void setup()
{
  // initialize serial communications
  Serial.begin(9600);

  // connection state
  boolean notConnected = true;

  // StartNB Module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(nb.begin(PINNUMBER)==NB_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");

  nb.shutdown();
  Serial.println("NB terminated");

}

void loop()
{
}
```

### `NB.getTime()`

#### Description

Get the time in seconds since January 1st, 1970. The time is retrieved from the NB module which periodically fetches the time from the cellular network you are attached to.

#### Syntax

```
NB.getTime();
```

#### Parameters
none

#### Returns
Returns the time in seconds since January 1st, 1970 on success. 0 on failure.

## NB_SMS Class

NB_SMS constructor

#### Description

NB_SMS is the base class for all NB functions relating to receiving and sending SMS messages.

### `beginSMS()`

#### Description

Identifies the telephone number to send an SMS message to.

#### Syntax

```
SMS.beginSMS(number)

```

#### Parameters
number : char array, the phone number to send the SMS to

#### Returns
int
In asynchronous mode, beginSMS() returns 0 if the last command is still executing, 1 if success, and >1 if there is an error. In synchronous mode, it returns 1 if it successfully executes, and 0 if it does not.

#### Example

```
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;

  // Start NB module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop() {

  Serial.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send SMS
  readSerial(remoteNum);
  Serial.println(remoteNum);

  // SMS text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
```

### `ready()`

#### Description

Gets the status if the last NB_SMS command.

#### Syntax

```
SMS.ready()

```

#### Parameters
none

#### Returns
int
In asynchronous mode, ready() returns 0 if the last command is still executing, 1 if it was successful, and >1 if there is an error. In synchronous mode, it returns 1 if the previous successfully executed, and 0 if it has not.

### `endSMS()`

#### Description

Tells the modem that the SMS message is complete and sends it.

#### Syntax

```
SMS.endSMS()

```

#### Parameters
none

#### Returns
int
In asynchronous mode, endSMS() returns 0 if it is still executing, 1 if successful, and >1 if there is an error. In synchronous mode, it returns 1 if the previous successfully executed, and 0 if it has not.

#### Example

```

// Include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;

  // Start NB module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop() {

  Serial.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send SMS
  readSerial(remoteNum);
  Serial.println(remoteNum);

  // SMS text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
```

### `available()`

#### Description

Checks to see if there is a SMS messages on the SIM card to be read, and reports back the number of characters in the message.

#### Syntax

```
SMS.available()

```

#### Parameters
none

#### Returns
int : the number of characters in the message

#### Example

```
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

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
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
```

### `remoteNumber()`

#### Description

Retrieves the phone number an from an incoming SMS message and stores it in a named array.

#### Syntax

```
SMS.remoteNumber(number, size)

```

#### Parameters
- number : char array, a named array that will hold the sender's number
- size : int, the size of the array

#### Returns
- int

In asynchronous mode, remoteNumber() returns 0 if the last command is still executing, 1 if success, and >1 if there is an error. In synchronous mode, it returns 1 if it successfully executes, and 0 if it does not.

#### Example

```
// include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

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
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
```

### `read()`

#### Description

Reads a byte from an SMS message. read() inherits from the Stream utility class.

#### Syntax

```
SMS.read()

```

#### Parameters
none

#### Returns
int - the first byte of incoming serial data available (or -1 if no data is available)

#### Example

```
// include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

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
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
```

### `write()`

#### Description

Writes a character to a SMS message.

#### Syntax

```
SMS.write(val)

```

#### Parameters
val: a character to send in the message

#### Returns
byte - write() will return the number of bytes written, though reading that number is optional

#### Example

```
// Include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;

  // Start NB module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop() {

  Serial.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send SMS
  readSerial(remoteNum);
  Serial.println(remoteNum);

  // SMS text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
```

### `print()`

#### Description

Writes a char array as a SMS message.

#### Syntax

```
SMS.print(message)

```

#### Parameters
message - char array, the message to send

#### Returns
int : the number of bytes printed

#### Example

```
// Include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;

  // Start NB module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    if (nbAccess.begin(PINNUMBER) == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop() {

  Serial.print("Enter a mobile number: ");
  char remoteNum[20];  // telephone number to send SMS
  readSerial(remoteNum);
  Serial.println(remoteNum);

  // SMS text
  Serial.print("Now, enter SMS content: ");
  char txtMsg[200];
  readSerial(txtMsg);
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
```

### `peek()`

#### Description

Returns the next byte (character) of an incoming SMS without removing it from the message. That is, successive calls to peek() will return the same character, as will the next call to read(). peek() inherits from the Stream utility class.

#### Syntax

```
SMS.peek()

```

#### Parameters
none

#### Returns
int - the first byte available of a SMS message (or -1 if no data is available)

#### Example

```
// include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

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
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
```

### `flush()`

#### Description

flush() clears the modem memory of any sent messages once all outgoing characters have been sent. flush() inherits from the Stream utility class.

#### Syntax

```
SMS.flush()

```

#### Parameters
none

#### Returns
none

#### Example

```
// include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instances
NB nbAccess;
NB_SMS sms;

// Array to hold the number an SMS is retrieved from
char senderNumber[20];

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Receiver");

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
  Serial.println("Waiting for messages");
}

void loop() {
  int c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while ((c = sms.read()) != -1) {
      Serial.print((char)c);
    }

    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);

}
```

## GPRS Class

### `GPRS constructor`

#### Description
GPRS is the base class for all GPRS functions, such as internet client and server behaviors.

### `attachGPRS()`

#### Description

Connects to the NarrowBand IoT or LTE Cat M1 to initiate GPRS communication.

Every cellular provider has an Access Point Name (APN) that serves as a bridge between the cellular network and the internet. Sometimes, there is a username and password associated with the connection point, but the NB technology relies on other information to authenticate the connection, therefore you don't need to specify anything because your SIM and the Network will establish the connection securely without any parameter given manually.

#### Syntax

```
gprs.attachGPRS()

```

#### Parameters
None

#### Returns
ERROR, IDLE, CONNECTING, NB_READY, GPRS_READY, TRANSPARENT_CONNECTED

#### Example

```
/*
  Web client

  This sketch connects to a website through a MKR NB 1500 board. Specifically,
  this example downloads the URL "http://example.org/" and
  prints it to the Serial monitor.

  Circuit:
   - MKR NB 1500 board
   - Antenna
   - SIM card with a data plan

  created 8 Mar 2012
  by Tom Igoe
*/

// libraries
#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path and port (for example: example.org)
char server[] = "example.org";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

  // After starting the modem with NB.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    Serial.print((char)client.read());
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;)
      ;
  }
}
```

## NBClient and NBSSLClient Class

### `Client`

#### Description

Client is the base class for all GPRS client based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `SSL Client`

#### Description

SSL Client is the base class for all GPRS SSL client based calls. It is not called directly, but invoked whenever you use a function that relies on it.

### `ready()`

#### Description

Gets the status of the last command

#### Syntax

```
client.ready()

```

#### Parameters
none

#### Returns
In asynchronous mode, ready() returns 0 if the last command is still executing, 1 if it was successful, and >1 if there is an error. In synchronous mode, it returns 1 if the previous successfully executed, and 0 if it has not.

### `connect()`

#### Description

Connects to a specified IP address and port. The return value indicates success or failure.

#### Syntax

```
client.connect(ip, port)

```

#### Parameters
ip: the IP address that the client will connect to (array of 4 bytes)
port: the port that the client will connect to (int)
#### Returns
boolean : Returns true if the connection succeeds, false if not.

#### Example

```
/*
  Web client

 This sketch connects to a website through a MKR NB 1500 board. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it
 to the Serial monitor.

 Circuit:
 * MKR NB 1500
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/NBExamplesWebClient

 */

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
     if((nbAccess.begin(PINNUMBER)==NB_READY) &&
       (gprs.attachGPRS()==GPRS_READY))
     notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `beginWrite()`

#### Description

Tells the client to start writing to the server it is connected to.

#### Syntax

```
client.beginWrite()

```

#### Parameters
none

#### Returns
none

### `write()`

#### Description

Write data to the server the client is connected to.

#### Syntax

```
client.write(data)
client.write(buffer)
client.write(buffer, size)
```

#### Parameters
data: the value to write (byte or char)
buffer : an array of data (byte or char) to write
size : size of the buffer to write (byte)
#### Returns
byte - write() returns the number of bytes written. It is not necessary to read this.

### `endWrite()`

#### Description

Stops writing data to a server

#### Syntax

```
client.endWrite()

```

#### Parameters
none

#### Returns
none

### `connected()`

#### Description

Returns whether or not the client is connected. A client is considered connected if the connection has been closed but there is still unread data.

#### Syntax

```
client.connected()

```

#### Parameters
none

#### Returns
boolean - Returns true if the client is connected, false if not.

#### Example

```
/*
  Web client

 This sketch connects to a website through a MKR NB 1500. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it
 to the Serial monitor.

 Circuit:
 * MKR NB 1500
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/NBExamplesWebClient

 */

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
      if((nbAccess.begin(PINNUMBER)==NB_READY) &&
       (gprs.attachGPRS()==GPRS_READY))
     notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `read()`

#### Description

Read the next byte received from the server the client is connected to (after the last call to read()).

read() inherits from the Stream utility class.

#### Syntax

```
client.read()

```

#### Parameters
none

#### Returns
int - The next byte (or character), or -1 if none is available.

#### Example

```
/*
  Web client

 This sketch connects to a website through a MKR NB 1500. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it
 to the Serial monitor.

 Circuit:
 * MKR NB 1500
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/NBExamplesWebClient

 */

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with NB.begin()
  // attach the module to the GPRS network with the APN, login and password
  while(notConnected)
  {
     if((nbAccess.begin(PINNUMBER)==NB_READY) &&
       (gprs.attachGPRS()==GPRS_READY))
     notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `available()`

#### Description

Returns the number of bytes available for reading (that is, the amount of data that has been written to the client by the server it is connected to).

available() inherits from the Stream utility class.

#### Syntax

```
client.available()

```

#### Parameters
none

#### Returns
The number of bytes available.

#### Example

```
/*
  Web client

 This sketch connects to a website through a MKR NB 1500. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it
 to the Serial monitor.

 Circuit:
 * MKR NB 1500
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/GSMExamplesWebClient

 */

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((nbAccess.begin(PINNUMBER)==NB_READY) &&
       (gprs.attachGPRS()==GPRS_READY))
     notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

### `peek()`

#### Description

Returns the next byte (character) of an incoming message removing it from the message. That is, successive calls to peek() will return the same character, as will the next call to read(). peek() inherits from the Stream utility class.

#### Syntax

```
client.peek()

```

#### Parameters
none

#### Returns
int - the next byte in an incoming message.

### `flush()`

#### Description

Discards any bytes that have been written to the client but not yet read.

flush() inherits from the Stream utility class.

#### Syntax

```
client.flush()

```

#### Parameters
none

#### Returns
none

### `stop()`

#### Description

Disconnects from the server

#### Syntax

```
client.stop()

```

#### Parameters
none

#### Returns
none

#### Example

```
/*
  Web client

 This sketch connects to a website through a MKR NB 1500. Specifically,
 this example downloads the URL "http://arduino.cc/" and prints it
 to the Serial monitor.

 Circuit:
 * MKR NB 1500
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

 http://arduino.cc/en/Tutorial/NBExamplesWebClient

 */

// libraries
#include <MKRNB.h>

// PIN Number
#define PINNUMBER ""

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path & port (for example: arduino.cc)
char server[] = "arduino.cc";
char path[] = "/";
int port = 80; // port 80 is the default for HTTP

void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  Serial.println("Starting Arduino web client.");
  // connection state
  boolean notConnected = true;

  // After starting the modem with nb.begin()
  // attach the module to the GPRS network with the APN, login and password
  while(notConnected)
  {
     if((nbAccess.begin(PINNUMBER)==NB_READY) &&
       (gprs.attachGPRS()==GPRS_READY))
     notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.0");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
}
```

## NBModem Class

### `NBModem Constructor`

#### Description

NBModem is the base class for calls that have specific diagnostic functionality with the modem. It is not called directly, but invoked whenever you use a function that relies on it.

### `begin()`

#### Description

Checks the modem status, and restarts it. Call this before getIMEI().

#### Syntax

```
modem.begin()

```

#### Parameters
none

#### Returns
int : returns 1 if modem is OK, otherwise returns an error.


### `getIMEI()`

#### Description

Retrieves the modem's IMEI number. Call this after begin().

#### Syntax

```
modem.getIMEI()

```

#### Parameters
none

#### Returns
String : the modem's IMEI number

#### Example

```
// libraries
#include <MKRNB.h>

// modem verification object
NBModem modem;

// IMEI variable
String IMEI = "";

void setup()
{
  // initialize serial communications
  Serial.begin(9600);

  // start modem test (reset and check response)
  Serial.print("Starting modem test...");
  if(modem.begin())
    Serial.println("modem.begin() succeeded");
  else
    Serial.println("ERROR, no modem answer.");
}

void loop()
{
  // get modem IMEI
  Serial.print("Checking IMEI...");
  IMEI = modem.getIMEI();

  // check IMEI response
  if(IMEI != NULL)
  {
    // show IMEI in serial monitor
    Serial.println("Modem's IMEI: " + IMEI);
    // reset modem to check booting:
    Serial.print("Resetting modem...");
    modem.begin();
    // get and check IMEI one more time
    if(modem.getIMEI() != NULL)
    {
      Serial.println("Modem is functioning properly");
    }
    else
    {
      Serial.println("Error: getIMEI() failed after modem.begin()");
    }
  }
  else
  {
    Serial.println("Error: Could not get IMEI");
  }
  // do nothing:
  while(true);
}
```

### `getICCID()`

#### Description

Retrieves the SIM's ICCID number. Call this after begin().

#### Syntax

```
modem.getICCID()

```

#### Parameters
none

#### Returns
String : the SIM's ICCID number

#### Example

```

// libraries
#include <MKRNB.h>

// modem verification object
NBModem modem;

// ICCID variable
String ICCID = "";

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start modem  (reset and check response)
  Serial.print("Starting modem test...");
  if (modem.begin()) {
    Serial.println("modem.begin() succeeded");
  } else {
    Serial.println("ERROR, no modem answer.");
    while (1);
  }

  // get ICCID
  Serial.print("ICCID = ");
  Serial.println(modem.getICCID());
}

void loop() {
}
```

## NBScanner Class

### `NBScanner Constructor`

#### Description

NBScanner is the base class for calls that have specific diagnostic functionality relating to scanning for available networks. It is not called directly, but invoked whenever you use a function that relies on it.

#### Functions

- `begin()`
- `getCurrentCarrier()`
- `getSignalStrength()`
- `readNetworks()`

### `begin()`

#### Description

Resets modem hardware.

#### Syntax

```
scanner.begin()

```

#### Parameters
none

#### Returns
int : returns 1 if modem is OK, otherwise returns an error.

### `getCurrentCarrier()`

#### Description

Gets and returns the name of the current network carrier.

#### Syntax

```
scanner.getCurrentCarrier()

```

#### Parameters
none

#### Returns
String : name of the current network carrier

getSignalStrength()`

#### Description

Gets and returns the strength of the signal of the network the modem is attached to.

#### Syntax

```
scanner.getSignalStrength()

```

#### Parameters
none

#### Returns
String : signal strength in 0-31 scale. 31 means power > 51dBm. 99=not detectable

readNetworks()`

#### Description

Searches for available carriers, and returns a list of them.

#### Syntax

```
scanner.readNetworks()

```

#### Parameters
none

#### Returns
String : A string with list of networks available

## NBPIN Class

### `NBPIN constructor`

#### Description

NBPIN is the base class for all NB based functions that deal with interacting with the PIN on the SIM card.

#### Functions

- `begin()`
- `isPIN()`
- `checkPIN()`
- `checkPUK()`
- `changePIN()`
- `switchPIN()`
- `checkReg()`
- `getPINUsed()`
- `setPINUsed()`

### `begin()`

#### Description

Checks the modem status, and restarts it.

#### Syntax

```
NBPIN.begin()

```

#### Parameters
none

#### Returns
int : returns 1 if modem is OK, otherwise returns an error.

### `isPIN()`

#### Description

Checks the SIM card to see if it is locked with a PIN.

#### Syntax

```
pin.isPIN()

```

#### Parameters
none

#### Returns
int : 0 if PIN lock is off, 1 if PIN lock is on, -1 if PUK lock is on, -2 if error exists.

### `checkPIN()`

#### Description

Queries the SIM card with a PIN number to see if it is valid.

#### Syntax

```
pin.checkPIN(PIN)

```

#### Parameters
PIN : String with the PIN number to check

#### Returns
int : Returns 0 if the PIN is valid, returns -1 if it is not.

### `checkPUK()`

#### Description

Queries the SIM card with a PUK and PIN number to see if it is valid.

#### Syntax

```
pin.checkPIN(PUK, PIN)

```

#### Parameters
- PIN : String with the PIN number to check
- PUK : String with the PUK number to check

#### Returns
int : Returns 0 if it is valid, returns -1 if it is not.

### `changePIN()`

#### Description

Changes the PIN number of a SIM, after verifying the existing one.

#### Syntax

```
pin.changePIN(oldPIN, newPIN)

```

#### Parameters
- oldPIN : String with the existing PIN number 
- newPIN : String with the desired PIN number

#### Returns
none

### `switchPIN()`

#### Description

Change PIN lock status.

#### Syntax

```
pin.switchPIN(pin)

```

#### Parameters
pin : String with the existing PIN number

#### Returns
none

### `checkReg()`

#### Description

Check if modem was registered in GPRS network

#### Syntax

```
pin.checkReg()

```

#### Parameters
none

#### Returns
int : 0 if modem was registered, 1 if modem was registered in roaming, -1 if error exists

### `getPinUsed()`

#### Description

Check if PIN lock is used.

#### Syntax

```
pin.getPinUsed()

```

#### Parameters
none

#### Returns
boolean : TRUE id locked, FALSE if not

### `setPinUsed()`

#### Description

Set PIN lock status.

#### Syntax

```
pin.setPinUsed(used)

```

#### Parameters
used : boolean, TRUE to lock the PIN, FALSE to unlock.

#### Returns
none

## NBUDP Class

### `NBUDP`

#### Description
Creates a named instance of the GSM UDP class that can send and receive UDP messages.

#### Syntax

```
NBUDP

```

#### Parameters
none

### `begin()`

#### Description
Initializes the NB UDP library and network settings. Starts NBUDP socket, listening at local port PORT .

#### Syntax

```
NBUDP.begin(port);
```

#### Parameters
port: the local port to listen on (int)

#### Returns
1: if successful
0: if there are no sockets available to use

### `available()`

#### Description

Get the number of bytes (characters) available for reading from the buffer. This is data that's already arrived.

This function can only be successfully called after NBUDP.parsePacket().

available() inherits from the Stream utility class.

#### Syntax

```
NBUDP.available()

```

#### Parameters
None

#### Returns
the number of bytes available in the current packet
0: if NBUDP.parsePacket() hasn't been called yet

### `beginPacket()`

#### Description
Starts a connection to write UDP data to the remote connection

#### Syntax

```
NBUDP.beginPacket(hostName, port);
NBUDP.beginPacket(hostIp, port);

```

#### Parameters
- hostName: the address of the remote host. It accepts a character string or an IPAddress
- hostIp: the IP address of the remote connection (4 bytes)
- port: the port of the remote connection (int)

#### Returns
1: if successful
0: if there was a problem with the supplied IP address or port

### `endPacket()`

#### Description
Called after writing UDP data to the remote connection. It finishes off the packet and send it.

#### Syntax

```
NBUDP.endPacket();
```

#### Parameters
None

#### Returns
1: if the packet was sent successfully
0: if there was an error

### `write()`

#### Description
Writes UDP data to the remote connection. Must be wrapped between beginPacket() and endPacket(). beginPacket() initializes the packet of data, it is not sent until endPacket() is called.

#### Syntax

```
NBUDP.write(byte);
NBUDP.write(buffer, size);

```

#### Parameters
byte: the outgoing byte
buffer: the outgoing message
size: the size of the buffer
#### Returns
single byte into the packet
bytes size from buffer into the packet

### `parsePacket()`

#### Description
It starts processing the next available incoming packet, checks for the presence of a UDP packet, and reports the size. parsePacket() must be called before reading the buffer with UDP.read().

#### Syntax

```
NBUDP.parsePacket();
```

#### Parameters
None

#### Returns
the size of the packet in bytes
0: if no packets are available

### `peek()`

#### Description
Read a byte from the file without advancing to the next one. That is, successive calls to peek() will return the same value, as will the next call to read().

This function inherited from the Stream class. See the Stream class main page for more information.

#### Syntax

```
NBUDP.peek()

```

#### Parameters
none

#### Returns
b: the next byte or character
-1: if none is available

### `read()`

#### Description
Reads UDP data from the specified buffer. If no arguments are given, it will return the next character in the buffer.

This function can only be successfully called after NBUDP.parsePacket().

#### Syntax

```
NBUDP.read();
NBUDP.read(buffer, len);
```

#### Parameters
- buffer: buffer to hold incoming packets (char*)
- len: maximum size of the buffer (int)

#### Returns
- b: the characters in the buffer (char)
- size: the size of the buffer
- -1: if no buffer is available

### `flush()`

#### Description
Discard any bytes that have been written to the client but not yet read.

`flush()` inherits from the Stream utility class.

#### Syntax

```
NBUDP.flush()

```

#### Parameters
none

#### Returns
none

### `stop()`

#### Description

Disconnect from the server. Release any resource being used during the UDP session.

#### Syntax

```
NBUDP.stop()

```

#### Parameters
none

#### Returns
none

### `remoteIP()`

#### Description
Gets the IP address of the remote connection.

This function must be called after NBUDP.parsePacket().

#### Syntax

```
NBUDP.remoteIP();
```

#### Parameters
None

#### Returns
4 bytes : the IP address of the host who sent the current incoming packet

### `remotePort()`

#### Description
Gets the port of the remote UDP connection.

This function must be called after NBUDP.parsePacket().

#### Syntax

```
NBUDP.remotePort();
```

#### Parameters
None

#### Returns
The port of the host who sent the current incoming packet
