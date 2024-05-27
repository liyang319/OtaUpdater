#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
// COUT << "Kewell Midware!" << endl;
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include "HttpUtility.h"
#include "Utility.h"

#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include <thread>
#include <chrono>
// #include "Base.h"

using namespace rapidjson;
using namespace std;

#define DEFAULT_VERSION_PATH "../app/VERSION"
#define DEVICE_SN "123456789"
#define APP_NAME "ControlBox"
#define DEFAULT_OTA_SAVE_PATH "/home/app/ota_back/ControlBox"
#define DEFAULT_APP_PATH "/home/app/ControlBox"

int i = 0;

int KillApp(std::string processName)
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

int DoOTA(std::string json)
{
    // 解析JSON字符串
    rapidjson::Document document;
    document.Parse(json.c_str());
    std::string outputFile = DEFAULT_OTA_SAVE_PATH;

    // 检查解析是否成功
    if (!document.IsObject())
    {
        std::cout << "解析失败！" << std::endl;
        return 0;
    }

    // 从JSON中获取值
    std::string status = document["status"].GetString();
    std::string newVer = document["newVer"].GetString();
    std::string url = document["url"].GetString();
    std::string md5 = document["md5"].GetString();

    // 打印获取的值
    std::cout << "状态: " << status << std::endl;
    std::cout << "是否有新版本: " << newVer << std::endl;
    std::cout << "URL: " << url << std::endl;
    std::cout << "MD5: " << md5 << std::endl;
    if (newVer == "true")
    {
        cout << "===============" << endl;
        int downloadRes = HttpUtility::httpdownload(url, outputFile);
        if (downloadRes == CURLE_OK)
        {
            std::cout << "File downloaded to: " << outputFile << std::endl;
            // downld OK
            sleep(1);
            KillApp(APP_NAME);
            sleep(1);
            Utility::ReplaceFileWithCmd(DEFAULT_APP_PATH, DEFAULT_OTA_SAVE_PATH);
            sleep(1);
            Utility::RunFile(DEFAULT_APP_PATH);
        }
    }
    return 1;
}

void OtaCheck()
{
    std::cout << "=========OtaCheck=======" << i++ << std::endl;
    string strVer = Utility::getFileContent(DEFAULT_VERSION_PATH);

    std::map<string, string> mapParam = {
        {"cmd", "otacheck"},
        {"version", strVer},
        {"sn", DEVICE_SN}};

    string strParam = HttpUtility::buildQueryString(mapParam);
    std::string response;
    cout << strParam << endl;
    CURLcode getRes = HttpUtility::httpget("http://192.168.80.235:8000/otacheck", strParam, response, 1000);
    if (getRes == CURLE_OK)
    {
        std::cout << "Get request successful" << std::endl;
        std::cout << "Response: " << response << std::endl;
        DoOTA(response);
    }
}

int main()
{
    while (true)
    {
        OtaCheck();
        // std::this_thread::sleep_for(std::chrono::hours(1));
        std::this_thread::sleep_for(std::chrono::seconds(100));
    }
}
