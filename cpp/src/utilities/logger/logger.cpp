#include "logger.h"


puma::Logger::Logger() {
    log="";
    active = true;
    generateRunFolder();
}

puma::Logger::Logger(std::string folderLabel) {
    log="";
    active = true;
    generateRunFolder(folderLabel);
}

puma::Logger::Logger(bool active) {
    this->active = active;
    log="";
}

puma::Logger::Logger(std::string folderLabel, bool active) {
    this->active = active;
    log="";
    generateRunFolder(folderLabel);
}

bool puma::Logger::generateRunFolder() {

    if(!active) { return true; }

        generateFolderName();

        std::cout << "Logs folder generated at: " << folderName << std::endl;
        return generateDirectory(folderName);
}

bool puma::Logger::generateRunFolder(std::string folderLabel) {

    if(!active) { return true; }

        generateFolderName();
        folderName.append(folderLabel);
        return generateDirectory(folderName);
}

void puma::Logger::generateFolderName() {
    
    char buff[FILENAME_MAX]; //create string buffer to hold path
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);

    folderName = current_working_dir;
    folderName.append("/logs");

    logLocation = folderName;
    logLocation.append("/puma_log_");

    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::string s;
    s.append(std::to_string(1900 + ltm->tm_year));
    if (std::to_string(1+ltm->tm_mon).length() == 1){
        s.append("0" + std::to_string(1+ltm->tm_mon));
    } else {
        s.append(std::to_string(1+ltm->tm_mon));
    }
    if (std::to_string(ltm->tm_mday).length() == 1){
        s.append("0" + std::to_string(ltm->tm_mday));
    } else {
        s.append(std::to_string(ltm->tm_mday));
    }
    s.append("_");
    if (std::to_string(ltm->tm_hour).length() == 1){
        s.append("0" + std::to_string(ltm->tm_hour));
    } else {
        s.append(std::to_string(ltm->tm_hour));
    }
    if (std::to_string(ltm->tm_min).length() == 1){
        s.append("0" + std::to_string(ltm->tm_min));
    } else {
        s.append(std::to_string(ltm->tm_min));
    }
    if (std::to_string(ltm->tm_sec).length() == 1){
        s.append("0" + std::to_string(ltm->tm_sec));
    } else {
        s.append(std::to_string(ltm->tm_sec));
    }
    logLocation.append(s);
    logLocation.append(".txt");   
}

bool puma::Logger::generateDirectory(std::string directoryPath) {
    int dir_err;

    std::string str = "mkdir -p ";

    str.append(directoryPath);
    dir_err = system(str.c_str());
    if (-1 == dir_err)
    {
        printf("Error creating directory!n");
        return false;
    }
    return true;
}

bool puma::Logger::generateSubFolder(std::string subFolderName) {
    std::string subFolderPath = folderName;
    subFolderPath.append("/");
    subFolderPath.append(subFolderName);
    return generateDirectory(subFolderPath);
}

void puma::Logger::appendLogSection(std::string sectionName) {
    log.append("\n\n ########## ");
    log.append(sectionName);
    log.append(" ##########\n");
}

void puma::Logger::appendLogLine(std::string logLine) {
    log.append(logLine);
    log.append("\n");
}

void puma::Logger::appendLogItem(std::string logItem) {
    log.append(logItem);
}

void puma::Logger::appendLogItem(int v) {
    log.append(puma::PString::fromInt(v));
}

void puma::Logger::appendLogItem(long v) {
    log.append(puma::PString::fromLong(v));
}

void puma::Logger::appendLogItem(float v) {
    log.append(puma::PString::fromFloat(v));
}

void puma::Logger::appendLogItem(double v) {
    log.append(puma::PString::fromDouble(v));
}

void puma::Logger::appendLogItem(char v) {
    std::string s;
    s.push_back(v);
    log.append(s);
}

void puma::Logger::appendLogItemAndSpace(std::string item) {
    log.append(item);
    log.append(" ");
}

void puma::Logger::appendLogItemAndComma(std::string item) {
    log.append(item);
    log.append(",");
}

void puma::Logger::newLine() {
    log.append("\n");
}

std::string puma::Logger::getTime() {
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string s = dt;
    s = replaceAll(s, "\n","");
    return s;
}

bool puma::Logger::writeLog() {
    if(!active) { return true; }

    std::ofstream myfile;

    myfile.open(logLocation);
    myfile << log;
    myfile.close();

    return true;
}

void puma::Logger::emptyLog() {
    if(!active) { return; }

    log="";
    generateRunFolder();

}

void puma::Logger::emptyLog(std::string folderLabel) {
    if(!active) { return; }

    log="";
    generateRunFolder(folderLabel);

}

std::string puma::Logger::replaceAll(std::string baseString, std::string searchFor, std::string replaceWith) {
    while( baseString.find(searchFor) != std::string::npos) {
        baseString.replace(baseString.find(searchFor),searchFor.length(),replaceWith);
    }
    return baseString;
}
