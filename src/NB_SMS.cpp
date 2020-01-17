/*
  This file is part of the MKR NB library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

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

#include "NB_SMS.h"

enum {
  SMS_STATE_IDLE,
  SMS_STATE_LIST_MESSAGES,
  SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE
};

NB_SMS::NB_SMS(bool synch) :
#ifndef NO_SMS_CHARSET
  _bufferUTF8({0,0,0,0}),
  _indexUTF8(0),
  _ptrUTF8(""),
#endif
  _synch(synch),
  _state(SMS_STATE_IDLE),
  _smsTxActive(false)
{
}

#ifndef NO_SMS_CHARSET
/* Translation tables from GSM_03.38 are equal to UTF-8 for the
 * positions 0x0A, 0x0D, 0x1B, 0x20-0x23, 0x25-0x3F, 0x41-0x5A, 0x61-0x7A.
 * Collect the others into two translation tables.
 * Code will be much simpler if basic table contains all entries for
 * 0x00-0x24, 0x3B-0x40, 0x5B-0x60, 0x7B-0x7F */

struct sms_mapping {
  const unsigned char smsc;
  const char *utf8;

  sms_mapping(const char smsc, const char *utf8)
    : smsc(smsc), utf8(utf8) {}
};

sms_mapping _smsUTF8map[] = {
    {0x00,"@"},{0x10,"Δ"},{0x20," "},{0x40,"¡"},{0x60,"¿"},
    {0x01,"£"},{0x11,"_"},{0x21,"!"},
    {0x02,"$"},{0x12,"Φ"},{0x22,"\""},
    {0x03,"¥"},{0x13,"Γ"},{0x23,"#"},
    {0x04,"è"},{0x14,"Λ"},{0x24,"¤"},
    {0x05,"é"},{0x15,"Ω"},
    {0x06,"ù"},{0x16,"Π"},
    {0x07,"ì"},{0x17,"Ψ"},
    {0x08,"ò"},{0x18,"Σ"},
    {0x09,"Ç"},{0x19,"Θ"},
/* Text mode SMS uses 0x1A as send marker so Ξ is not available. */
    {0x0A,"\r"},//{0x1A,"Ξ"},
    {0x0B,"Ø"},{0x1B,"\b"},{0x3B,";"},{0x5B,"Ä"},{0x7B,"ä"},
    {0x0C,"ø"},{0x1C,"Æ"},{0x3C,"<"},{0x5C,"Ö"},{0x7C,"ö"},
    {0x0D,"\n"},{0x1D,"æ"},{0x3D,"="},{0x5D,"Ñ"},{0x7D,"ñ"},
    {0x0E,"Å"},{0x1E,"ß"},{0x3E,">"},{0x5E,"Ü"},{0x7E,"ü"},
    {0x0F,"å"},{0x1F,"É"},{0x3F,"?"},{0x5F,"§"},{0x7F,"à"}};

/* Text mode SMS uses 0x1B as abort marker so extended set is not available. */
#if 0
sms_mapping _smsXUTF8map[] = {
    {0x40,"|"},
    {0x14,"^"},
    {0x65,"€"},
    {0x28,"{"},
    {0x29,"}"},
    {0x0A,"\f"},
    {0x1B,"\b"},
    {0x3C,"["},
    {0x0D,"\n"},{0x3D,"~"},
    {0x3E,"]"},
    {0x2F,"\\"}};
 */
#endif
#endif

size_t NB_SMS::write(uint8_t c)
{
  if (_smsTxActive) {
#ifndef NO_SMS_CHARSET
    if (c <= 0x24 || c >= 0x80 || (c&0x1F) == 0 || (c&0x1F) >= 0x1B) {
      _bufferUTF8[_indexUTF8++]=c;
      if (_bufferUTF8[0] < 0x80
          || (_indexUTF8==2 && (_bufferUTF8[0]&0xE0) == 0xC0)
          || (_indexUTF8==3 && (_bufferUTF8[0]&0xF0) == 0xE0)
          || _indexUTF8==4) {
        for (auto &smschar : _smsUTF8map) {
          if (strncmp(_bufferUTF8, smschar.utf8, _indexUTF8)==0) {
            _indexUTF8=0;
            return MODEM.write(smschar.smsc);
          }
        }
        // No UTF8 match, echo buffer
        for (c=0; c < _indexUTF8; MODEM.write(_bufferUTF8[c++]));
        _indexUTF8=0;
      }
      return 1;
    }
#endif
    return MODEM.write(c);
  }

  return 0;
}

int NB_SMS::beginSMS(const char* to)
{
  MODEM.sendf("AT+CMGS=\"%s\"", to);
  if (MODEM.waitForResponse(100) == 2) {
    _smsTxActive = false;

    return (_synch) ? 0 : 2;
  }

#ifndef NO_SMS_CHARSET
  _indexUTF8=0;
#endif
  _smsTxActive = true;

  return 1;
}

int NB_SMS::ready()
{
  int ready = MODEM.ready();

  if (ready == 0) {
    return 0;
  }

  switch(_state) {
    case SMS_STATE_IDLE:
    default: {
      break;
    }

    case SMS_STATE_LIST_MESSAGES: {
      MODEM.setResponseDataStorage(&_incomingBuffer);
      MODEM.send("AT+CMGL=\"REC UNREAD\"");
      _state = SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE;
      ready = 0;
      break;
    }

    case SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE: {
      _state = SMS_STATE_IDLE;
      break;
    }
  }

  return ready;
}

int NB_SMS::endSMS()
{
  int r;

  if (_smsTxActive) {
#ifndef NO_SMS_CHARSET
    // Echo remaining content of UTF8 buffer
    for (r=0; r < _indexUTF8; MODEM.write(_bufferUTF8[r++]));
    _indexUTF8=0;
#endif
    MODEM.write(26);

    if (_synch) {
      r = MODEM.waitForResponse(3*60*1000);
    } else {
      r = MODEM.ready();
    }

    return r;
  } else {
    return (_synch ? 0 : 2);
  }
}

int NB_SMS::available()
{
  int nextMessageIndex = _incomingBuffer.indexOf("+CMGL: ");

  if (nextMessageIndex != -1) {
    _incomingBuffer.remove(0, nextMessageIndex);
  } else {
    _incomingBuffer = "";
  }

  if (_incomingBuffer.length() == 0) {
    int r;

    if (_state == SMS_STATE_IDLE) {
      _state = SMS_STATE_LIST_MESSAGES;
    }

    if (_synch) {
      unsigned long start = millis();
      while ((r = ready()) == 0 && (millis() - start) < 3*60*1000) {
        delay(100);
      }
    } else {
      r = ready();
    }

    if (r != 1) {
      return 0;
    } 
  }

  if (_incomingBuffer.startsWith("+CMGL: ")) {

    _incomingBuffer.remove(0, 7);

    _smsDataIndex = _incomingBuffer.indexOf('\n') + 1;

    _smsDataEndIndex = _incomingBuffer.indexOf("\r\n+CMGL: ",_smsDataIndex);
    if (_smsDataEndIndex == -1) {
      _smsDataEndIndex = _incomingBuffer.length() - 1;
    }

    return (_smsDataEndIndex - _smsDataIndex) + 1;
  } else {
    _incomingBuffer = "";
  }

  return 0;
}

int NB_SMS::remoteNumber(char* number, int nlength)
{
  #define PHONE_NUMBER_START_SEARCH_PATTERN "\"REC UNREAD\",\""
  int phoneNumberStartIndex = _incomingBuffer.indexOf(PHONE_NUMBER_START_SEARCH_PATTERN);

  if (phoneNumberStartIndex != -1) {
    int i = phoneNumberStartIndex + sizeof(PHONE_NUMBER_START_SEARCH_PATTERN) - 1;

    while (i < (int)_incomingBuffer.length() && nlength > 1) {
      char c = _incomingBuffer[i];

      if (c == '"') {
        break;
      }

      *number++ = c;
      nlength--;
      i++;
    }

    *number = '\0';
    return 1;
  } else {
    *number = '\0';
  }

  return 2;
}

int NB_SMS::read()
{
#ifndef NO_SMS_CHARSET
  if (*_ptrUTF8 != 0) {
    return *_ptrUTF8++;
  }
  if (_smsDataIndex < _incomingBuffer.length() && _smsDataIndex <= _smsDataEndIndex) {
    char c = _incomingBuffer[_smsDataIndex++];
    if (c <= 0x24 || c >= 0x80 || (c&0x1F) == 0 || (c&0x1F) >= 0x1B) {
      for (auto &smschar : _smsUTF8map) {
        if (c == smschar.smsc) {
          _ptrUTF8=smschar.utf8;
          return *_ptrUTF8++;
        }
      }
    }
    return c;
#else
  int bufferLength = _incomingBuffer.length();

  if (_smsDataIndex < bufferLength && _smsDataIndex <= _smsDataEndIndex) {
    return _incomingBuffer[_smsDataIndex++];
#endif
  }

  return -1;
}

int NB_SMS::peek()
{
#ifndef NO_SMS_CHARSET
  if (*_ptrUTF8 != 0) {
    return *_ptrUTF8;
  }
  if (_smsDataIndex < _incomingBuffer.length() && _smsDataIndex <= _smsDataEndIndex) {
    char c = _incomingBuffer[_smsDataIndex];
    if (c <= 0x24 || c >= 0x80 || (c&0x1F) == 0 || (c&0x1F) >= 0x1B) {
      for (auto &smschar : _smsUTF8map) {
        if (c == smschar.smsc) {
          return smschar.utf8[0];
        }
      }
    }
    return c;
#else
  if (_smsDataIndex < (int)_incomingBuffer.length() && _smsDataIndex <= _smsDataEndIndex) {
    return _incomingBuffer[_smsDataIndex];
#endif
  }

  return -1;
}

void NB_SMS::flush()
{
  int smsIndexEnd = _incomingBuffer.indexOf(',');

#ifndef NO_SMS_CHARSET
  _ptrUTF8 = "";
#endif
  if (smsIndexEnd != -1) {
    while (MODEM.ready() == 0);

    MODEM.sendf("AT+CMGD=%s", _incomingBuffer.substring(0, smsIndexEnd).c_str());

    if (_synch) {
      MODEM.waitForResponse(55000);
    }
  }
}
