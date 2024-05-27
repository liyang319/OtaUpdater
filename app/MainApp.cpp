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

#include <openssl/md5.h>
#include <iomanip>
#include <sstream>

// #include "Base.h"

using namespace rapidjson;
// maeusing namespace std;

#define DEFAULT_VERSION_PATH "../app/VERSION"
#define DEVICE_SN "123456789"
#define APP_NAME "ControlBox"
// #define DEFAULT_OTA_SAVE_PATH "/home/app/ota_save/ControlBox"
// #define DEFAULT_APP_PATH "/home/app/ControlBox"
#define DEFAULT_APP_RIGHTS "777"
#define DEFAULT_OTA_SAVE_PATH "/Users/yli/Desktop/Kewell/KewellMidware/server/ota_save/ControlBox"
#define DEFAULT_APP_PATH "/Users/yli/Desktop/Kewell/KewellMidware/server/ControlBox"

int i = 0;
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
        std::cout << "===============" << std::endl;
        int downloadRes = HttpUtility::httpdownload(url, outputFile);
        if (downloadRes == CURLE_OK)
        {
            std::cout << "File downloaded to: " << outputFile << std::endl;
            // downld OK
            sleep(1);
            Utility::changeFileMode(DEFAULT_OTA_SAVE_PATH, DEFAULT_APP_RIGHTS);
            Utility::killApp(APP_NAME);
            sleep(1);
            Utility::replaceFileWithCmd(DEFAULT_APP_PATH, DEFAULT_OTA_SAVE_PATH);
            sleep(1);
            Utility::runFile(DEFAULT_APP_PATH, true);
        }
    }
    return 1;
}

void OtaCheck()
{
    std::cout << "=========OtaCheck=======" << i++ << std::endl;
    std::string strVer = Utility::getFileContent(DEFAULT_VERSION_PATH);

    std::map<std::string, std::string> mapParam = {
        {"cmd", "otacheck"},
        {"version", strVer},
        {"sn", DEVICE_SN}};

    std::string strParam = HttpUtility::buildQueryString(mapParam);
    std::string response;
    std::cout << strParam << std::endl;
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
    // while (true)
    // {
    //     OtaCheck();
    //     // std::this_thread::sleep_for(std::chrono::hours(1));
    //     std::this_thread::sleep_for(std::chrono::seconds(100));
    // }
    // std::string md5 = get_file_md5("./ControlBox");
    // std::string md5 = Utility::calculateMD5("./ControlBox");
    // Utility::RunFile(DEFAULT_APP_PATH, true);
    // printf("------md5--%s-\n", md5.c_str());
}
