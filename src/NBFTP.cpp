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

#include "NBFTP.h"

// REFERENCE doc for most of the commands and info
// see FTP section starting at page 241
// https://www.u-blox.com/en/docs/UBX-17003787

enum {
    CLIENT_STATE_IDLE,
    CLIENT_STATE_WAIT_CONNECT_RESPONSE,
    CLIENT_STATE_WAIT_CONNECT_URC,
    CLIENT_STATE_WAIT_COMMAND_RESPONSE,
    CLIENT_STATE_WAIT_COMMAND_URC,
    CLIENT_STATE_WAIT_RECV_RESPONSE,
    CLIENT_STATE_WAIT_RECV_URC,
    CLIENT_STATE_RECV_DATA,
    CLIENT_STATE_WAIT_SEND_RESPONSE,
    CLIENT_STATE_WAIT_SEND_URC,
    CLIENT_STATE_SEND_DATA,
    CLIENT_STATE_WAIT_SEND_DISCONNECT,
    CLIENT_STATE_WAIT_SEND_CLEANUP,
    CLIENT_STATE_CLOSE,
    CLIENT_STATE_WAIT_CLOSE_RESPONSE,
    CLIENT_STATE_RETRIEVE_ERROR
};

/** Constructor
     @param synch Sync mode
*/
NBFTP::NBFTP(bool synch) :
_synch(synch),
_connected(false),
_state(CLIENT_STATE_IDLE)
{
    MODEM.addUrcHandler(this);
}

/** Destructor
 */
NBFTP::~NBFTP()
{
    MODEM.removeUrcHandler(this);
}

/** Get last command status
 * if synch = false, you must call until you receive a non-zero to complete command
 * execution. All other commands will return 0 until ready() returns >0.
 * If synch = true, the function will not return until ready becomes >0 (and therefore the command has completed)
     @return returns 0 if last command is still executing, 1 success, >1 error
*/
int NBFTP::ready()
{
    int ready = MODEM.ready();

    if (ready == 0) {
        // modem is not finished processing
        // lots of reasons, see the modem.ready() function
        return 0;
    }

    switch (_state) {
        // in these states, we aren't doing much,
        // so if the modem is ready, we are ready
        case CLIENT_STATE_IDLE:
        case CLIENT_STATE_RECV_DATA:
        case CLIENT_STATE_SEND_DATA:
        default: {
            break;
        }

        // between connecting and being connected
        // we first get an OK message, then a +UUFTPCR
        // this is awaiting the OK. If we get ERROR, ready>1,
        // if we got OK, ready=1, and we move onto waiting for the +UUFTPCR
        case CLIENT_STATE_WAIT_CONNECT_RESPONSE: {
            if (ready > 1) {
                _state = CLIENT_STATE_CLOSE;
            } else {
                _state = CLIENT_STATE_WAIT_CONNECT_URC;
            }
            ready = 0;
            break;
        }

        // between sending a command and completing,
        // we first receive an OK/ERROR, this is indicated by
        // the ready variable here. Once we receive OK, we 
        // wait for the URC (+UUFTPCR) message.
        case CLIENT_STATE_WAIT_COMMAND_RESPONSE: {
            if (ready > 1) {
                _state = CLIENT_STATE_CLOSE;
            } else {
                _state = CLIENT_STATE_WAIT_COMMAND_URC;
            }
            ready = 0;
            break;
        }

        // send and receive requires an independent set of states
        // to catch the OK/ERROR and the URC. the logic
        // is nearly identical, but the route is through
        // different states.
        case CLIENT_STATE_WAIT_RECV_RESPONSE: {
            if (ready > 1) {
                _state = CLIENT_STATE_CLOSE;
            } else {
                _state = CLIENT_STATE_WAIT_RECV_URC;
            }
            ready = 0;
            break;
        }
        case CLIENT_STATE_WAIT_SEND_RESPONSE: {
            if (ready > 1) {
                _state = CLIENT_STATE_CLOSE;
            } else {
                _state = CLIENT_STATE_WAIT_SEND_URC;
            }
            ready = 0;
            break;
        }

        // in these states we return 0, meaning
        // that the command is not complete. The logic for these
        // is included in the URC handler.
        case CLIENT_STATE_WAIT_CONNECT_URC:
        case CLIENT_STATE_WAIT_COMMAND_URC:
        case CLIENT_STATE_WAIT_RECV_URC:
        case CLIENT_STATE_WAIT_SEND_URC:
        case CLIENT_STATE_WAIT_SEND_DISCONNECT:
        case CLIENT_STATE_WAIT_SEND_CLEANUP:
        {
            // in these states we are awaiting a URCHandler call
            // and that call holds the logic to update the state
            // until we receive that call we stay in this state
            // when we receive that call we will either error
            // or go back to idle.
            ready = 0;
            break;
        }

        // this state starts the close procedure.
        // disconnects from the server
        case CLIENT_STATE_CLOSE: {
            MODEM.send("AT_UFTPC=0");
            _connected = false;
            _state = CLIENT_STATE_WAIT_CLOSE_RESPONSE;
            ready = 0;
            break;
        }

        // when we receiving the OK/ERROR we 
        // go and check if there was an error
        // for debugging purposes and reporting...
        case CLIENT_STATE_WAIT_CLOSE_RESPONSE: {
            _state = CLIENT_STATE_RETRIEVE_ERROR;
            ready = 0;
            break;
        }

        // retreives the error for debugging
        case CLIENT_STATE_RETRIEVE_ERROR: {
            MODEM.send("AT+UFTPER");
            _state = CLIENT_STATE_IDLE;
            ready = 0;
            break;
        }
    }
    return ready;
}

/** configure all options at once, setting remainder to anonymous values
 * returns true only if all options are successfully set. If no arguments
 * are passed, FTP will be configured for an anonymous server in active mode 
 * without requiring an account.
 @param username Username for FTP login procedure. Max length = 30 chars
    @param password Password for FTP login procedure. Max length = 30 chars
    @param passive 0 value means active FTP, 1 value means passive FTP.
    All other values of passive cause undefined behaviour
    @param account Account for FTP login procedure. I have no idea what this is. Max length = 30 chars
    @return success or failure if all values are configured properly
*/
bool NBFTP::configure(const char* username, const char* password, uint8_t passive, const char* account) {
    return configureUsername(username) &&
    configurePassword(password) &&
    configurePassive(passive) &&
    configureAccount(account);
}

/** configure authentication options but don't change other options. 
 * sets to anonymous values if no arguments passed.
 * returns true only if all options are successfully set. If no arguments
 * are passed, FTP will be configured for anonymous connections (username anonymous, password guest, account blank)
 @param username Username for FTP login procedure. Max length = 30 chars
    @param password Password for FTP login procedure. Max length = 30 chars
    @param account Account for FTP login procedure. I have no idea what this is. Max length = 30 chars
    @return success or failure if all values are configured properly
*/
bool NBFTP::configureAuth(const char* username, const char* password, const char* account) {
    return configureUsername(username) &&
    configurePassword(password) && 
    configureAccount(account);
}

/** configure username without changing other configuration options
 * if no username is passed, sets to blank (all logins will fail
 * until reconfigured).
     @param username Username for FTP login procedure. Max length = 30 chars
    @return success or failure if username is configured
*/
bool NBFTP::configureUsername(const char* username) {
    return configureString(2, username);
}

/** configure password without changing other configuration options
 * if no password is passed, sets to blank (all logins will fail
 * until reconfigured).
     @param password Password for FTP login procedure. Max length = 30 chars
    @return success or failure if password is configured
*/
bool NBFTP::configurePassword(const char* password) {
    return configureString(3, password);
}

/** configure passive mode without changing other configuration options
 * if no argument is passed, sets to active mode. Pass 1 for passive mode,
 * 0 for active mode.
     @param passive 1 for passive mode, 0 for active mode
    @return success or failure if connection mode is configured
*/
bool NBFTP::configurePassive(uint8_t passive) {
    return configureInt(6, passive);
}

/** configure account without changing other configuration options
 * if no account is passed, default (blank) value is passed. I have no idea
 * what this does, but I support it anyways. Leaving blank works in most scenarios
     @param account Accounf for FTP login procedure. Max length = 30 chars
    @return success or failure if account is configured
*/
bool NBFTP::configureAccount(const char* account) {
    return configureString(4, account);
}

/** Connect to server by IP address
 * use configure functions to set account and connection type
     @param (IPAddress)
    @param port
    @return returns 0 if last command is still executing, 1 success, 2 if there was an error
*/
int NBFTP::connect(IPAddress ip, uint16_t port) {
    if (configureIP(ip) && configureInt(7, port)) {
        return connect();
    } else {
        return 2;
    }
}

/** Connect to server by IP address
 * use configure functions to set account and connection type
     @param host - max length 128 chars
    @param port
    @return returns 0 if last command is still executing, 1 success, 2 if there was an error
*/
int NBFTP::connect(const char *host, uint16_t port) {
    if (configureString(1, host) && configureInt(7, port)) {
        return connect();
    } else {
        return 2;
    }
}

/** Start an upload to a specific file. Use this to open the file
 * if synch is on, this will return true if the file is opened,
 * false if an error occured. If synch is false, you must wait
 * availableForWrite() to become 1.
 * returns 0 if still opening, 1 for success, 2 for error
 */
bool NBFTP::sendFileBegin(const uint8_t *file) {
    if (_synch) {
        while (ready() == 0);
    } else if (ready() == 0) {
        // if _synch is false, we won't run
        // if the previous command is still working
        return 0;
    }

    MODEM.sendf("AT+UFTPC=7,\"%s\"", file);
    _state = CLIENT_STATE_WAIT_SEND_RESPONSE;

    if (_synch) {
        while (ready() == 0) {
            delay(100);
        }
        return _state == CLIENT_STATE_SEND_DATA;
    }
    // client must call ready() until availableForWrite() is 1
    return 0;
}

/** Indicates if you are connected to a file opened for upload.
 * Only if this returns 1 should you use the write() functions.
 * return codes: 
 * - 0: connecting to a file, but not yet connected
 * - 1: connected to a file, and ready for upload
 * - 2: not connected nor connecting to a file
 */
int NBFTP::availableForWrite() {
    switch (_state) {
        case CLIENT_STATE_SEND_DATA:
            return 1;
        case CLIENT_STATE_WAIT_SEND_RESPONSE:
        case CLIENT_STATE_WAIT_SEND_URC:
            return 0;
        default:
            return 2;
    }
}

/** write the contents of the file being uploaded. Only available
 * following a successfull sendFileBegin() and before a sendFileEnd()
 * will return 0 if write is not available or failed. On success,
 * returns the number of bytes written. Bytes are not uploaded until 
 * a sendFileEnd() call is made.
     @param (uint8_t) character to be written to file being uploaded
    @return returns number of bytes written into the uploading file
*/
size_t NBFTP::write(uint8_t c) {
    if (_state != CLIENT_STATE_SEND_DATA) {
        return 0;
    }

    return MODEM.write(c);
}

/** write the contents of the file being uploaded. Only available
 * following a successfull sendFileBegin() and before a sendFileEnd()
 * will return 0 if write is not available or failed. On success,
 * returns the number of bytes written. Bytes are not uploaded until 
 * a sendFileEnd() call is made.
    @param buf array of characters to be written to file being uploaded
    @param size length of the buf array
    @return returns number of bytes written into the file
*/
size_t NBFTP::write(const uint8_t *buf, size_t size) {
    if (_state != CLIENT_STATE_SEND_DATA) {
        return 0;
    }

    // this could be updated but needs to adapt MODEM.write(buf,size)
    // to not wait for echo responses.
    int i;
    for (i = 0; i < size && write(buf[i]) == 1; i++);

    return i;
}

/** ends the connection to an uploading file
 * only should be called when availableForWrite() returns 1
 * will do nothing otherwise
 */
void NBFTP::sendFileEnd() {
    if (_state != CLIENT_STATE_SEND_DATA) {
        return;
    }
    delay(1000);
    write((uint8_t*) "+++", 3);
    _state = CLIENT_STATE_WAIT_SEND_DISCONNECT;

    if (_synch) {
        while (ready() == 0) {
            delay(100);
        }
    }
}

int NBFTP::getFileBegin(const uint8_t *file) {
    if (_synch) {
        while (ready() == 0);
    } else if (ready() == 0) {
        return 0;
    }

    MODEM.sendf("AT+UFTPC=6,%s", file);
    _state = CLIENT_STATE_WAIT_RECV_RESPONSE;

    if (_synch) {
        while (ready() == 0) {
            delay(100);
        }
    }
    return _state == CLIENT_STATE_RECV_DATA;
}

/** check if there are more bytes available to read from
 * the file being downloaded. returns 0 if no file is being downloaded,
 * and 1 if there are more bytes to donwload.
     @return returns 1 if there is more file to read, 0 if not
*/
int NBFTP::available() {
    if (_state != CLIENT_STATE_RECV_DATA) {
        return 0;
    }

    return (_peek != -1);
}

/** read a single character from file being downloaded.
 * returns -1 if no data is available. Be sure to check
 * available() before calling this function or you 
 * will receive additional zeros. Function only useful following a getFileBegin() and while
 * available() returns 1
 @return returns the next byte from the file being downloaded
*/
int NBFTP::read() {
    return 0;
}

/** reads as much of the file as is available into the buffer,
 * up to size bytes maximum. Will return number of bytes filled
 * in. all other bytes will remain untouched. A null byte is not guaranteed
 * to follow in the buffer. If the return value is less than size, you have
 * reached EOF. Function only useful following a getFileBegin() and while
 * available() returns 1
 @param buf an array for bytes to go into
    @param size size of the buffer
    @return returns the number of bytes read.
*/
int NBFTP::read(uint8_t *buf, size_t size) {
    size_t filled = 0;

    while (filled < size && available() == 1) {
        buf[filled++] = read();
    }
    return filled;
}

void NBFTP::endRead() {
    
}

/** reads the next byte in the file being downloaded.
 * Function only useful following a getFileBegin() and while
 * available() returns 1
     @return returns a peek at the next byte from the file being downloaded
*/
int NBFTP::peek() {
    if (_state != CLIENT_STATE_RECV_DATA) {
        return -1;
    }

    return _peek;
}
/** disconnect from server and close connection
 * any in progress transfers will attempt to be terminated
 */
void NBFTP::stop() {
    _state = CLIENT_STATE_IDLE;
    if (_connected) {
        MODEM.send("AT+UFTPC=0");
        MODEM.waitForResponse(10000);
    }

    _connected = false;
}

/** is there an open connection */
uint8_t NBFTP::connected() {
    return (uint8_t) _connected;
}

/** undefined behavior */
NBFTP::operator bool() {
    return true;
}

void NBFTP::handleUrc(const String& urc)
{
    Serial.print("URC!");
    Serial.println(urc);
    if (urc.startsWith("+UUFTPCR: ")) {
        // result value
        char result = urc.charAt(urc.lastIndexOf(',') + 1);
        if (result == '0') {
            // command failed. We check the state to see if this was
            // a connect command (because connect commands do not need to be 
            // followed by a close). Otherwise, we close the connection due to error
            Serial.println("Going to error state");
            _state = CLIENT_STATE_RETRIEVE_ERROR;
        } else if (result == '1') {
            // command succeeded. We check if it was a connect command and will
            // update connected here instead of in ready
            if (_state == CLIENT_STATE_WAIT_CONNECT_URC) {
                _connected = true;
            }
            _state = CLIENT_STATE_IDLE;
        } else {
            Serial.println("Panic!");
        }
    } else if (urc.startsWith("CONNECT")) {
        Serial.println("Got connect!");
        switch (_state) {
            case CLIENT_STATE_WAIT_RECV_URC: {
                _state = CLIENT_STATE_RECV_DATA;
                break;
            }
            case CLIENT_STATE_WAIT_SEND_URC: {
                _state = CLIENT_STATE_SEND_DATA;
                break;
            }
        }
    } else if (urc.startsWith("DISCONNECT")) {
        Serial.println("Got disconnect");
        _state = CLIENT_STATE_WAIT_SEND_CLEANUP;
    } else if (urc.startsWith("+UUFTPCD: ")) {
        Serial.println("Detected a +UUFTPCD: message");
    }
}

// private

  bool _synch;
  int _connected;
  int _state;

  bool NBFTP::configureString(int opcode, const char* str) {
      if (str[0] == 0) {
        MODEM.sendf("AT+UFTP=%d", opcode);
      } else {
        MODEM.sendf("AT+UFTP=%d,\"%s\"", opcode, str);
      }
      return (MODEM.waitForResponse(100) != 2);
  }
  bool NBFTP::configureInt(int opcode, int value) {
      MODEM.sendf("AT+UFTP=%d,%d", opcode, value);
      return (MODEM.waitForResponse(100) != 2);
  }

  bool NBFTP::configureIP(IPAddress ip) {
    MODEM.sendf("AT+UFTP=0,\"%d.%d.%d.%d\"", ip[0], ip[1], ip[2], ip[3]);
    return (MODEM.waitForResponse(100) != 2);
  }

  int NBFTP::connect() {
    Serial.println("Connecting");
    if (_synch) {
        while (ready() == 0);
    } else if (ready() == 0) {
        return 0;
    }

    if (_connected) {
        stop();
    }

    MODEM.send("AT+UFTPC=1");

    _state = CLIENT_STATE_WAIT_CONNECT_RESPONSE;

    if (_synch) {
        while (ready() == 0) {
            delay(100);
        }
    }

    return (_synch) ? _connected : 1;
  }
