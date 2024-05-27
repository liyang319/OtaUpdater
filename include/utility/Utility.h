#ifndef UTILITY_H
#define UTILITY_H

#include <string>

class Utility
{
public:
    static std::string getFileContent(std::string filename);
    // static std::string calculateMD5(const std::string &file_path);
    static int replaceFileWithCmd(std::string orginalFile, std::string newFile);
    static int replaceFile(const std::string orginalFile, const std::string newFile);
    static int runFile(std::string executablePath, bool bBackground);
    static int killApp(std::string processName);
    static int changeFileMode(std::string filename, std::string mode);
    static std::string calculateMD5(std::string filename);
};

#endif // UTILITY_H
