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

#include "Base.h"

using namespace rapidjson;
// maeusing namespace std;

// #define DEFAULT_VERSION_PATH "../app/VERSION"
#define DEFAULT_VERSION_PATH "/var/version"
#define DEVICE_SN "4854604D7765A027"
#define APP_NAME "ControlBox"
#define CONFIG_NAME "ControlBox.ini"
#define UPDATER_NAME "OtaUpdater"
#define APP_BASE_PATH "/home/app/"
#define DEFAULT_OTA_SAVE_PATH "/home/app/ota_save/"
#define DEFAULT_OTA_BACKUP_PATH "/home/app/ota_backup/"
#define DEFAULT_APP_PATH "/home/app/ControlBox"
#define DEFAULT_APP_RIGHTS "777"
// #define DEFAULT_OTA_SAVE_PATH "/Users/yli/Desktop/WorkCode/OtaUpdater/OtaUpdater/build/output/ota_save/"
// #define DEFAULT_OTA_BACKUP_PATH "/Users/yli/Desktop/WorkCode/OtaUpdater/OtaUpdater/build/output/ota_backup/"
// #define DEFAULT_APP_PATH "/Users/yli/Desktop/Kewell/KewellMidware/server/ControlBox"
#define URL_CHECK_OTA "http://192.168.80.235:8000/otacheck"
#define URL_UPLOAD_LOG "http://192.168.80.235:8000/upload"
#define URL_CHECK_LOG "http://192.168.80.235:8000/logcheck"
#define DEFAULT_SN_FILE_PATH "/var/sn"
#define LOGVAL_NEED_UPLOAD "upload"

// #define CONCAT(x, y) x##y
// #define FULL_PATH(x, y) CONCAT(x, y)

int i = 0;

void OtaRecovery()
{
}

void OtaBackup()
{
    Utility::copyFileTo(APP_NAME, DEFAULT_OTA_BACKUP_PATH);
    Utility::copyFileTo(CONFIG_NAME, DEFAULT_OTA_BACKUP_PATH);
    Utility::copyFileTo(UPDATER_NAME, DEFAULT_OTA_BACKUP_PATH);
}

int DoOTA(std::string json)
{
    // 解析JSON字符串
    rapidjson::Document document;
    document.Parse(json.c_str());

    // 检查解析是否成功
    if (!document.IsObject())
    {
        std::cout << "解析失败！" << std::endl;
        return 0;
    }

    // 从JSON中获取值
    std::string status = document["status"].GetString();
    std::string needUpdate = document["needUpdate"].GetString();
    std::string url = document["url"].GetString();
    std::string md5 = document["md5"].GetString();
    std::string newVer = document["newVer"].GetString();

    // 打印获取的值
    std::cout << "状态: " << status << std::endl;
    std::cout << "是否有新版本: " << needUpdate << std::endl;
    std::cout << "URL: " << url << std::endl;
    std::cout << "MD5: " << md5 << std::endl;
    std::cout << "newVer: " << newVer << std::endl;
    if (needUpdate == "true")
    {
        std::cout << "===============" << std::endl;
        // 创建升级包保存位置ota_save
        Utility::createDirIfNotExist(DEFAULT_OTA_SAVE_PATH);
        // 创建原程序备份位置ota_backup
        Utility::createDirIfNotExist(DEFAULT_OTA_BACKUP_PATH);

        std::string outputFile = DEFAULT_OTA_SAVE_PATH + Utility::getFilenameFromUrl(url);
        int downloadRes = HttpUtility::httpdownload(url, outputFile);
        if (downloadRes == CURLE_OK)
        {
            std::cout << "File downloaded to: " << outputFile << std::endl;
            // downld OK
            sleep(1);
            std::string otaMd5 = Utility::calculateMD5(outputFile);
            std::cout << " ----md5---- " << md5 << std::endl;
            if (md5 != otaMd5)
            {
                std::cout << " ----md5 fail---- " << std::endl;
                return 0;
            }
            // 备份原有版本到ota_backup
            // OtaBackup();
            COUT << "---------Unzip package-----------" << endl;
            Utility::unzipFile(outputFile, DEFAULT_OTA_SAVE_PATH);
            Utility::changeFileMode(std::string(DEFAULT_OTA_SAVE_PATH) + APP_NAME, DEFAULT_APP_RIGHTS);
            Utility::killApp(APP_NAME);
            sleep(1);
            std::cout << " ---------replace old version---- " << std::endl;
            Utility::replaceFileWithCmd(DEFAULT_APP_PATH, std::string(DEFAULT_OTA_SAVE_PATH) + APP_NAME);
            // Utility::fillVersionFile(DEFAULT_VERSION_PATH, newVer);
            sleep(5);
            int status = Utility::runFile(DEFAULT_APP_PATH, true);
            if (status == 0)
            {
                COUT << "启动成功" << endl;
            }
            else
            {
                COUT << "启动失败" << endl;
            }
            Utility::deleteDirectory(DEFAULT_OTA_SAVE_PATH);
        }
    }
    else
    {
        std::cout << " ----no new Version---- " << std::endl;
    }
    return 1;
}

void OtaCheck()
{
    std::cout << "=========OtaCheck=======" << i++ << std::endl;
    std::string strVer = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_VERSION_PATH));
    std::string deviceSN = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_SN_FILE_PATH));
    // std::string deviceSN = DEVICE_SN;

    std::map<std::string, std::string> mapParam = {
        {"cmd", "otacheck"},
        {"version", strVer},
        {"sn", DEVICE_SN}};

    std::string strParam = HttpUtility::buildQueryString(mapParam);
    std::string response;
    CURLcode getRes = HttpUtility::httpget(URL_CHECK_OTA, strParam, response, 1000);
    if (getRes == CURLE_OK)
    {
        std::cout << "Get request successful" << std::endl;
        std::cout << "Response: " << response << std::endl;
        DoOTA(response);
    }
}

int DoLogOperation(std::string json, std::string deviceSN)
{
    // 解析JSON字符串
    rapidjson::Document document;
    document.Parse(json.c_str());
    std::string outputFile = DEFAULT_OTA_SAVE_PATH;

    // 检查解析是否成功
    if (!document.IsObject())
    {
        COUT << "解析失败！" << endl;
        return 0;
    }

    // 从JSON中获取值
    std::string status = document["status"].GetString();
    std::string logStatus = document["log"].GetString();
    std::string path = document["path"].GetString();
    std::string logDate = document["date"].GetString();

    // 打印获取的值
    COUT << "状态: " << status << endl;
    COUT << "log: " << logStatus << endl;
    COUT << "path: " << path << endl;
    COUT << "date: " << logDate << endl;

    if (logStatus == LOGVAL_NEED_UPLOAD)
    {
        // Do upload here
        std::string logFileName = "controlbox_" + logDate + ".log";
        COUT << "-----logfile-----" << logFileName << endl;
        HttpUtility::httpUploadFile(URL_UPLOAD_LOG, logFileName, logFileName, deviceSN);
    }
}

void LogCheck()
{
    std::cout << "=========LogCheck=======" << i++ << std::endl;
    std::string strVer = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_VERSION_PATH));
    std::string deviceSN = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_SN_FILE_PATH));
    // std::string deviceSN = DEVICE_SN;

    std::map<std::string, std::string>
        mapParam = {
            {"cmd", "logcheck"},
            {"version", strVer},
            {"sn", deviceSN}};
    std::string strParam = HttpUtility::buildQueryString(mapParam);
    std::string response;
    CURLcode getRes = HttpUtility::httpget(URL_CHECK_LOG, strParam, response, 1000);
    if (getRes == CURLE_OK)
    {
        COUT << "Get request successful" << endl;
        COUT << "Response: " << response << endl;
        DoLogOperation(response, deviceSN);
    }
}

int main()
{
    int index = 0;
    // while (true)
    // {
    //     if (index % 10 == 0)
    //         OtaCheck();
    //     else
    //         LogCheck();
    //     // std::this_thread::sleep_for(std::chrono::hours(1));
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     COUT << "---------------" << index++ << endl;
    // }
    OtaCheck();
}
