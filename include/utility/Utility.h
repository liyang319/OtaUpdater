#ifndef UTILITY_H
#define UTILITY_H

#include <string>

class Utility
{
public:
    static std::string getFileContent(std::string filename);
    static std::string calculateMD5(const std::string &file_path);
    static int ReplaceFileWithCmd(std::string orginalFile, std::string newFile);
    static int ReplaceFile(const std::string orginalFile, const std::string newFile);
    static int RunFile(std::string executablePath, bool bBackground);
};

#endif // UTILITY_H
