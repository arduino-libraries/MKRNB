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

#ifndef _NB_FTP2_H_INCLUDED
#define _NB_FTP2_H_INCLUDED

#include "Modem.h"

#include <Client.h>

class NBFTP : public Client, public ModemUrcHandler {
public:
  /** Constructor
      @param synch Sync mode
   */
  NBFTP(bool synch = true);

  /** Destructor
   */
  virtual ~NBFTP();

  /** Get last command status
   * if synch = false, you must call until you receive a non-zero to complete command
   * execution. All other commands will return 0 until ready() returns >0.
   * If synch = true, the function will not return until ready becomes >0 (and therefore the command has completed)
      @return returns 0 if last command is still executing, 1 success, >1 error
  */
  virtual int ready();

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
  bool configure(const char* username = "anonymous", const char* password = "guest", uint8_t passive = 0, const char* account = "");

  /** configure authentication options but don't change other options. 
   * sets to anonymous values if no arguments passed.
   * returns true only if all options are successfully set. If no arguments
   * are passed, FTP will be configured for anonymous connections (username anonymous, password guest, account blank)
     @param username Username for FTP login procedure. Max length = 30 chars
     @param password Password for FTP login procedure. Max length = 30 chars
     @param account Account for FTP login procedure. I have no idea what this is. Max length = 30 chars
     @return success or failure if all values are configured properly
   */
  bool configureAuth(const char* username = "anonymous", const char* password = "guest", const char* account = "");

  /** configure username without changing other configuration options
   * if no username is passed, sets to blank (all logins will fail
   * until reconfigured).
      @param username Username for FTP login procedure. Max length = 30 chars
      @return success or failure if username is configured
   */
  bool configureUsername(const char* username = "");

  /** configure password without changing other configuration options
   * if no password is passed, sets to blank (all logins will fail
   * until reconfigured).
      @param password Password for FTP login procedure. Max length = 30 chars
      @return success or failure if password is configured
   */
  bool configurePassword(const char* password = "");
  
  /** configure passive mode without changing other configuration options
   * if no argument is passed, sets to active mode. Pass 1 for passive mode,
   * 0 for active mode.
      @param passive 1 for passive mode, 0 for active mode
      @return success or failure if connection mode is configured
   */
  bool configurePassive(uint8_t passive = 0);

  /** configure account without changing other configuration options
   * if no account is passed, default (blank) value is passed. I have no idea
   * what this does, but I support it anyways. Leaving blank works in most scenarios
      @param account Accounf for FTP login procedure. Max length = 30 chars
      @return success or failure if account is configured
   */
  bool configureAccount(const char* account);
  
  /** Connect to server by IP address
   * use configure functions to set account and connection type
      @param (IPAddress)
      @param port
      @return returns 0 if last command is still executing, 1 success, 2 if there was an error
  */
  int connect(IPAddress ip, uint16_t port = 21);

  /** Connect to server by IP address
   * use configure functions to set account and connection type
      @param host - max length 128 chars
      @param port
      @return returns 0 if last command is still executing, 1 success, 2 if there was an error
  */
  int connect(const char *host, uint16_t port = 21);

  /** Start an upload to a specific file. Use this to open the file
   * if synch is on, this will return true if the file is opened,
   * false if an error occured. If synch is false, you must wait
   * availableForWrite() to become 1.
   */
  bool sendFileBegin(const uint8_t *file);

  /** Indicates if you are connected to a file opened for upload.
   * Only if this returns 1 should you use the write() functions.
   * return codes: 
   * - 0: connecting to a file, but not yet connected
   * - 1: connected to a file, and ready for upload
   * - 2: not connected nor connecting to a file
   */
  int availableForWrite();

  /** write the contents of the file being uploaded. Only available
   * following a successfull sendFileBegin() and before a sendFileEnd()
   * will return 0 if write is not available or failed. On success,
   * returns the number of bytes written. Bytes are not uploaded until 
   * a sendFileEnd() call is made.
      @param (uint8_t) character to be written to file being uploaded
      @return returns number of bytes written into the uploading file
   */
  size_t write(uint8_t);

  /** write the contents of the file being uploaded. Only available
   * following a successfull sendFileBegin() and before a sendFileEnd()
   * will return 0 if write is not available or failed. On success,
   * returns the number of bytes written. Bytes are not uploaded until 
   * a sendFileEnd() call is made.
      @param buf array of characters to be written to file being uploaded
      @param size length of the buf array
      @return returns number of bytes written into the uploading file
   */
  size_t write(const uint8_t *buf, size_t size);

  void sendFileEnd();

  int getFileBegin(const uint8_t *file);

  /** check if there are more bytes available to read from
   * the file being downloaded. returns 0 if no file is being downloaded,
   * and 1 if there are more bytes to donwload.
      @return returns 1 if there is more file to read, 0 if not
   */
  int available();

  /** read a single character from file being downloaded.
   * returns -1 if no data is available. Be sure to check
   * available() before calling this function or you 
   * will receive additional zeros. Function only useful following a getFileBegin() and while
   * available() returns 1
     @return returns the next byte from the file being downloaded
   */
  int read();
  
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
  int read(uint8_t *buf, size_t size);

  void endRead();

  /** reads the next byte in the file being downloaded.
   * Function only useful following a getFileBegin() and while
   * available() returns 1
      @return returns a peek at the next byte from the file being downloaded
   */
  int peek();
  /** function not valid. All writes immediately flush */
  void flush() {};
  /** disconnect from server and close connection
   * any in progress transfers will attempt to be terminated
   */
  void stop();

  /** is there an open connection */
  uint8_t connected();

  /** undefined behavior */
  operator bool();

  void getError();

  virtual void handleUrc(const String& urc);
private:
  bool _synch;
  int _connected;
  int _state;
  int _peek;
  uint8_t terminator;

  bool configureString(int opcode, const char* str);
  bool configureInt(int opcode, int value);
  bool configureIP(IPAddress ip);
  int connect();
};

#endif