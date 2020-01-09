/*
  This file is part of the MKR NB library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Modem.h"

#define MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS 20

ModemUrcHandler* ModemClass::_urcHandlers[MAX_URC_HANDLERS] = { NULL };
Print* ModemClass::_debugPrint = NULL;

ModemClass::ModemClass(Uart& uart, unsigned long baud, int resetPin, int powerOnPin) :
  _uart(&uart),
  _baud(baud),
  _resetPin(resetPin),
  _powerOnPin(powerOnPin),
  _lastResponseOrUrcMillis(0),
  _atCommandState(AT_COMMAND_IDLE),
  _ready(1),
  _responseDataStorage(NULL)
{
  _buffer.reserve(64);
}

int ModemClass::begin(bool restart)
{
  _uart->begin(_baud > 115200 ? 115200 : _baud);

  // power on module
  pinMode(_powerOnPin, OUTPUT);
  digitalWrite(_powerOnPin, HIGH);

  if (_resetPin > -1 && restart) {
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, HIGH);
    delay(100);
    digitalWrite(_resetPin, LOW);
  } else {
    if (!autosense()) {
      return 0;
    }

    if (!reset()) {
      return 0;
    }
  }

  if (!autosense()) {
    return 0;
  }

  if (_baud > 115200) {
    sendf("AT+IPR=%ld", _baud);
    if (waitForResponse() != 1) {
      return 0;
    }

    _uart->end();
    delay(100);
    _uart->begin(_baud);

    if (!autosense()) {
      return 0;
    }
  }

  return 1;
}

void ModemClass::end()
{
  _uart->end();
  digitalWrite(_resetPin, HIGH);

  // power off module
  digitalWrite(_powerOnPin, LOW);
}

void ModemClass::debug()
{
  debug(Serial);
}

void ModemClass::debug(Print& p)
{
  _debugPrint = &p;
}

void ModemClass::noDebug()
{
  _debugPrint = NULL;
}

int ModemClass::autosense(unsigned long timeout)
{
  for (unsigned long start = millis(); (millis() - start) < timeout;) {
    if (noop() == 1) {
      return 1;
    }

    delay(100);
  }

  return 0;
}

int ModemClass::noop()
{
  send("AT");

  return (waitForResponse() == 1);
}

int ModemClass::reset()
{
  send("AT+CFUN=15");

  return (waitForResponse(1000) == 1);
}

size_t ModemClass::write(uint8_t c)
{
  return _uart->write(c);
}

size_t ModemClass::write(const uint8_t* buf, size_t size)
{
  size_t result = _uart->write(buf, size);

  // the R410m echos the binary data, when we don't what it to so
  size_t ignoreCount = 0;

  while (ignoreCount < result) {
    if (_uart->available()) {
      _uart->read();

      ignoreCount++;
    }
  }

  return result;
}

void ModemClass::send(const char* command)
{
  // compare the time of the last response or URC and ensure
  // at least 20ms have passed before sending a new command
  unsigned long delta = millis() - _lastResponseOrUrcMillis;
  if(delta < MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS) {
    delay(MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS - delta);
  }

  _uart->println(command);
  _uart->flush();
  _atCommandState = AT_COMMAND_IDLE;
  _ready = 0;
}

void ModemClass::sendf(const char *fmt, ...)
{
  char buf[BUFSIZ];

  va_list ap;
  va_start((ap), (fmt));
  vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
  va_end(ap);

  send(buf);
}

int ModemClass::waitForPrompt(unsigned long timeout)
{
  for (unsigned long start = millis(); (millis() - start) < timeout;) {
    while (_uart->available()) {
      char c = _uart->read();
      if (_debugPrint) {
        _debugPrint->print(c);
      }

      _buffer += c;

      if (_buffer.endsWith(">")) {
        return 1;
      }
    }
  }
  return -1;
}

int ModemClass::waitForResponse(unsigned long timeout, String* responseDataStorage)
{
  _responseDataStorage = responseDataStorage;
  for (unsigned long start = millis(); (millis() - start) < timeout;) {
    int r = ready();

    if (r != 0) {
      _responseDataStorage = NULL;
      return r;
    }
  }

  _responseDataStorage = NULL;
  _buffer = "";
  return -1;
}

int ModemClass::ready()
{
  poll();

  return _ready;
}

void ModemClass::poll()
{
  while (_uart->available()) {
    char c = _uart->read();

    if (_debugPrint) {
      _debugPrint->write(c);
    }

    _buffer += c;

    switch (_atCommandState) {
      case AT_COMMAND_IDLE:
      default: {

        if (_buffer.startsWith("AT") && _buffer.endsWith("\r\n")) {
          _atCommandState = AT_RECEIVING_RESPONSE;
          _buffer = "";
        }  else if (_buffer.endsWith("\r\n")) {
          _buffer.trim();

          if (_buffer.length()) {
            _lastResponseOrUrcMillis = millis();

            for (int i = 0; i < MAX_URC_HANDLERS; i++) {
              if (_urcHandlers[i] != NULL) {
                _urcHandlers[i]->handleUrc(_buffer);
              }
            }
          }

          _buffer = "";
        }

        break;
      }

      case AT_RECEIVING_RESPONSE: {
        if (c == '\n') {
          _lastResponseOrUrcMillis = millis();

          int endOfResponse = 0;
          if (_buffer.startsWith("+CMGL: ")) {
            // SMS responses can contain "OK\r\n"
            for(int nextSMS = 0; nextSMS != -1; nextSMS = _buffer.indexOf("+CMGL: ",endOfResponse)) {
              // First line is SMS info
              nextSMS = _buffer.indexOf("\r\n",nextSMS);
              // Second line is SMS content
              endOfResponse = _buffer.indexOf("\r\n",nextSMS);
              if (endOfResponse == -1) {
                // Not yet complete SMS
                break;
              }
            }
          }

          int responseResultIndex;

          responseResultIndex = _buffer.lastIndexOf("OK\r\n");
        
          if (responseResultIndex >= endOfResponse) {
            _ready = 1;
          } else {
            responseResultIndex = _buffer.lastIndexOf("ERROR\r\n");
            if (responseResultIndex >= endOfResponse) {
              _ready = 2;
            } else {
              responseResultIndex = _buffer.lastIndexOf("NO CARRIER\r\n");
              if (responseResultIndex >= endOfResponse) {
                _ready = 3;
              } else {
                responseResultIndex = _buffer.lastIndexOf("CME ERROR");
                if (responseResultIndex >= endOfResponse) {
                  _ready = 4;
                }
              }
            }
          }

          if (_ready != 0) {
            if (_responseDataStorage != NULL) {
              if (_ready > 1) {
                _buffer.substring(responseResultIndex);
              } else {
                _buffer.remove(responseResultIndex);
              }
              _buffer.trim();

              *_responseDataStorage = _buffer;

              _responseDataStorage = NULL;
            }

            _atCommandState = AT_COMMAND_IDLE;
            _buffer = "";
            return;
          }
        }
        break;
      }
    }
  }
}

void ModemClass::setResponseDataStorage(String* responseDataStorage)
{
  _responseDataStorage = responseDataStorage;
}

void ModemClass::addUrcHandler(ModemUrcHandler* handler)
{
  for (int i = 0; i < MAX_URC_HANDLERS; i++) {
    if (_urcHandlers[i] == NULL) {
      _urcHandlers[i] = handler;
      break;
    }
  }
}

void ModemClass::removeUrcHandler(ModemUrcHandler* handler)
{
  for (int i = 0; i < MAX_URC_HANDLERS; i++) {
    if (_urcHandlers[i] == handler) {
      _urcHandlers[i] = NULL;
      break;
    }
  }
}

void ModemClass::setBaudRate(unsigned long baud)
{
  _baud = baud;
}

ModemClass MODEM(SerialSARA, 115200, SARA_RESETN, SARA_PWR_ON);
