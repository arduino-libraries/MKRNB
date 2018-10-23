#include "MKRNB.h"
#include "GSMFileUtils.h"
#include "update.h"

GSMFileUtils fileManager(true);

unsigned long baud = 115200;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  // put your setup code here, to run once:
  fileManager.begin(10000);
  fileManager.downloadFile("OTA", delta_pkg_modem_05_06_99_05_06_bin, delta_pkg_modem_05_06_99_05_06_bin_len, FOAT,  true, false);
  listFiles(fileManager);
  fileManager.fwUpdate();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    uint8_t c = Serial.read();
    Serial2.write(c);
  }

  while (Serial2.available())
  {
    uint8_t c = Serial2.read();
    Serial.write(c);
  }

  // check if the USB virtual serial wants a new baud rate
  if (Serial.baud() != baud) {
    baud = Serial.baud();
    Serial2.begin(baud);
  }
}
