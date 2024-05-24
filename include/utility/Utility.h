#ifndef UTILITY_H
#define UTILITY_H

#include <string>

class Utility
{
public:
    static std::string getFileContent(std::string filename);
    static std::string calculateMD5(const std::string &file_path);
};

#endif // UTILITY_H
