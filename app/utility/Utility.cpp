#include "Utility.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// std::string Utility::getFileContent(const std::string &filename)
// {
//     std::ifstream file(filename);
//     if (file.is_open())
//     {
//         std::string content;
//         std::string line;
//         while (std::getline(file, line))
//         {
//             content += line + "\n";
//         }
//         file.close();
//         return content;
//     }
//     else
//     {
//         std::cerr << "Error opening file " << filename << std::endl;
//         return "";
//     }
// }

std::string Utility::getFileContent(std::string fileName)
{
    std::ifstream file(fileName);
    std::string content;
    std::string line;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            content += line;
            if (!file.eof())
            {
                content += "\n";
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << fileName << std::endl;
    }

    return content;
}

std::string Utility::calculateMD5(const std::string &file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        return "";
    }

    uint32_t s[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
    uint32_t k[] = {0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE};
    uint32_t a, b, c, d, f, g, temp;
    uint32_t buffer[16];

    uint64_t file_size = 0;
    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    while (!file.eof())
    {
        file.read(reinterpret_cast<char *>(buffer), 64);
        uint64_t bytes_read = file.gcount();
        file_size -= bytes_read;
        if (bytes_read < 64)
        {
            buffer[bytes_read] = 0x80;
            while (bytes_read++ < 56)
            {
                buffer[bytes_read] = 0;
            }
            buffer[14] = file_size << 3;
            buffer[15] = file_size >> 29;
        }

        a = s[0];
        b = s[1];
        c = s[2];
        d = s[3];

        for (int i = 0; i < 64; i++)
        {
            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            temp = d;
            d = c;
            c = b;
            b = b + ROTATE_LEFT((a + f + k[i / 16] + buffer[g]), 7);
            a = temp;
        }

        s[0] += a;
        s[1] += b;
        s[2] += c;
        s[3] += d;
    }

    std::stringstream stream;
    for (int i = 0; i < 4; i++)
    {
        stream << std::hex << std::setw(8) << std::setfill('0') << s[i];
    }

    return stream.str();
}

int Utility::ReplaceFileWithCmd(std::string orginalFile, std::string newFile)
{
    std::string command = "cp " + newFile + " " + orginalFile;
    int result = system(command.c_str()); // 执行命令行

    if (result == 0)
    {
        return 0; // 成功
    }
    else
    {
        return -1; // 失败
    }
}

int ReplaceFile(const std::string orginalFile, const std::string newFile)
{
    std::ifstream newFileInput(newFile);
    if (!newFileInput)
    {
        std::cerr << "Error: Unable to open new file" << std::endl;
        return -1;
    }

    std::ofstream orginalFileOutput(orginalFile);
    if (!orginalFileOutput)
    {
        std::cerr << "Error: Unable to open orginal file" << std::endl;
        return -1;
    }

    orginalFileOutput << newFileInput.rdbuf();

    return 0;
}

int Utility::RunFile(std::string executablePath)
{
    std::string command = "./" + executablePath; // 构建启动可执行文件的命令行
    int result = system(command.c_str());        // 执行命令行
    return result;
}