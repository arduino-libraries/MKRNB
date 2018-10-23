#ifndef _GSM_FILEUTILS_H_INCLUDED
#define _GSM_FILEUTILS_H_INCLUDED

#include <Arduino.h>

enum GSM_fileTags_t {
    USER = 0,
    FOAT = 1,
    PROFILE = 2
};

class GSMFileUtils {
public:
    GSMFileUtils(bool debug = false);

    void begin(unsigned long timeout = 10000);
    size_t fileCount() const { return _count; };
    void listFiles(String list[]) const;
    size_t listFile(const String filename);

    void downloadFile(const String filename, const char buf[], const size_t size, const GSM_fileTags_t tag = USER, const bool binary = false, const bool append = false);
    void downloadFile(const String filename, const String &buf, const GSM_fileTags_t tag = USER, const bool binary = false, const bool append = false) { downloadFile(filename, buf.c_str(), buf.length(), tag, binary, append); }
    void downloadBinary(const String filename, const char buf[], const size_t size, const GSM_fileTags_t tag = USER, const bool append = false) { downloadFile(filename, buf, size, tag, true, append); }
    void downloadBinary(const String filename, const String &buf, const GSM_fileTags_t tag = USER, const bool append = false) { downloadFile(filename, buf.c_str(), buf.length(), tag, true, append); }

    int deleteFile(const String filename);
    int deleteFiles();

    size_t readFile(const String filename, String *content, const bool binary = false);
    size_t readBinary(const String filename, String *content) { return readFile(filename, content, true); }

    void fwUpdate();
    size_t freeSpace();

private:
    int _count;
    String _files;
    const char *_fileTags[3];
    void _countFiles();
    int _getFileList();
};

void listFiles(const GSMFileUtils fileUtils);

#endif
