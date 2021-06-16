#ifndef PUMA_Logger_H
#define PUMA_Logger_H

#include "pstring.h"

#include <cstring>
#include <ctime>
#include <unistd.h>
#define GetCurrentDir getcwd


namespace puma {

class Logger
{
public:
    Logger();
    Logger(std::string folderLabel);
    Logger(bool active);
    Logger(std::string folderLabel, bool active);

    bool generateSubFolder(std::string subFolderName);

    void appendLogSection(std::string sectionName);
    void appendLogLine(std::string logLine);
    void appendLogItem(std::string item);
    void appendLogItem(int item);
    void appendLogItem(long item);
    void appendLogItem(float item);
    void appendLogItem(double item);
    void appendLogItem(char item);
    void appendLogItemAndSpace(std::string item);
    void appendLogItemAndComma(std::string item);
    void newLine();

    bool writeLog();

    void emptyLog();
    void emptyLog(std::string folderLabel);

    static std::string replaceAll(std::string baseString, std::string searchFor, std::string replaceWith);
    static std::string getTime();

    std::string log;
    std::string logLocation;
    std::string folderName;

private:

    bool active;

    bool generateRunFolder();
    bool generateRunFolder(std::string folderLabel);

    void generateFolderName();
    bool generateDirectory(std::string directoryPath);

};

}



#endif // PUMA_Logger_H
