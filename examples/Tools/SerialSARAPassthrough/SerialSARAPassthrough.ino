/*
   SerialSARAPassthrough sketch

   This sketch allows you to send AT commands from the USB CDC serial port
   of the MKR NB 1500 board to the onboard ublox SARA-R410 celluar module.

   For a list of supported AT commands see:
   https://www.u-blox.com/sites/default/files/u-blox-CEL_ATCommands_%28UBX-13002752%29.pdf

   Circuit:
   - MKR NB 1500 board
   - Antenna
   - 1500 mAh or higher lipo battery connected
   - SIM card

   Make sure the Serial Monitor's line ending is set to "Both NL and CR"

   create 11 December 2017
   Sandeep Mistry
*/

// baud rate used for both Serial ports
unsigned long baud = 115200;

void setup() {
  // enable the POW_ON pin
  pinMode(SARA_PWR_ON, OUTPUT);
  digitalWrite(SARA_PWR_ON, HIGH);

  // reset the ublox module
  pinMode(SARA_RESETN, OUTPUT);
  digitalWrite(SARA_RESETN, HIGH);
  delay(100);
  digitalWrite(SARA_RESETN, LOW);

  Serial.begin(baud);
  SerialSARA.begin(baud);
}

void loop() {
  if (Serial.available()) {
    SerialSARA.write(Serial.read());
  }

  if (SerialSARA.available()) {
    Serial.write(SerialSARA.read());
  }
}
