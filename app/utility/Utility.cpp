#include "Utility.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "openssl/md5.h"

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

std::string Utility::removeTrailingNewline(std::string str)
{
    std::string modifiedStr = str;
    if (!modifiedStr.empty() && modifiedStr.back() == '\n')
    {
        modifiedStr.erase(modifiedStr.length() - 1);
    }
    return modifiedStr;
}

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

int Utility::replaceFileWithCmd(std::string orginalFile, std::string newFile)
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

int replaceFile(const std::string orginalFile, const std::string newFile)
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

int Utility::runFile(std::string executablePath, bool bBackground)
{
    // std::string command = "./" + executablePath; // 构建启动可执行文件的命令行
    // int result = system(command.c_str());        // 执行命令行
    // return result;
    std::string command = executablePath; // 使用绝对路径启动可执行文件
    if (bBackground)
        command += " &";
    int result = system(command.c_str()); // 执行命令行
    return result;
}

int Utility::killApp(std::string processName)
{
    // std::string processName = "example_process";     // 要杀死的进程名字
    std::string command = "pkill -f " + processName; // 构造要执行的命令
    int result = system(command.c_str());            // 执行命令
    if (result == 0)
    {
        std::cout << "进程 " << processName << " 已成功被杀死。" << std::endl;
    }
    else
    {
        std::cerr << "无法杀死进程 " << processName << "。" << std::endl;
    }
    return result;
}

int Utility::changeFileMode(std::string filename, std::string mode)
{
    std::string command = "chmod " + mode + " " + filename;
    int result = system(command.c_str());
    if (result == 0)
    {
        std::cout << "文件权限修改成功" << std::endl;
    }
    else
    {
        std::cerr << "文件权限修改失败" << std::endl;
    }
    return result;
}

std::string Utility::calculateMD5(std::string filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return "";
    }

    MD5_CTX ctx;
    MD5_Init(&ctx);

    char buffer[1024];
    while (file)
    {
        file.read(buffer, sizeof(buffer));
        std::streamsize count = file.gcount();
        if (count > 0)
        {
            MD5_Update(&ctx, buffer, count);
        }
    }

    unsigned char md[MD5_DIGEST_LENGTH];
    MD5_Final(md, &ctx);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md[i]);
    }

    return ss.str();
}

void Utility::fillVersionFile(std::string filename, std::string content)
{
    // Open the file in out mode to clear the original content
    std::ofstream file(filename, std::ios::out | std::ios::trunc);

    if (file.is_open())
    {
        // Write the new content to the file
        file << content;

        // Close the file
        file.close();

        std::cout << "Content written to " << filename << " successfully." << std::endl;
    }
    else
    {
        std::cerr << "Error opening file " << filename << std::endl;
    }
}