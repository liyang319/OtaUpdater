#include "Utility.h"
#include <fstream>
#include <iostream>

std::string Utility::getFileContent(const std::string &filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string content;
        std::string line;
        while (std::getline(file, line))
        {
            content += line + "\n";
        }
        file.close();
        return content;
    }
    else
    {
        std::cerr << "Error opening file " << filename << std::endl;
        return "";
    }
}
