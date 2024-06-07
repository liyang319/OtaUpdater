#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
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
#include "Version.h"

using namespace rapidjson;
// maeusing namespace std;

// #define DEFAULT_VERSION_PATH "../app/VERSION"
#define DEFAULT_VERSION_PATH "/var/version"
#define DEVICE_SN "4854604D7765A027"
#define APP_NAME "ControlBox"
#define CONFIG_NAME "ControlBox.ini"
#define UPDATER_NAME "OtaUpdater"
#define RESTORE_SCRIPT_NAME "restore.sh"
#define UPDATE_REPLACE_SCRIPT_NAME "update_replace.sh"
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
#define MAX_LAUNCH_TRY_TIME 3

// #define CONCAT(x, y) x##y
// #define FULL_PATH(x, y) CONCAT(x, y)

int i = 0;

void OtaRecovery()
{
    COUT << "OtaRecovery" << endl;
    Utility::startApp(std::string(DEFAULT_OTA_SAVE_PATH) + RESTORE_SCRIPT_NAME, false);
}

void OtaBackup()
{
    COUT << "OtaBackup" << endl;
    Utility::copyFileTo(APP_NAME, DEFAULT_OTA_BACKUP_PATH);
    Utility::copyFileTo(CONFIG_NAME, DEFAULT_OTA_BACKUP_PATH);
    Utility::copyFileTo(UPDATER_NAME, DEFAULT_OTA_BACKUP_PATH);
}

void OtaReplace()
{
    // Utility::killApp(APP_NAME);
    sleep(1);
    COUT << "Replace old version" << std::endl;
    // Utility::replaceFileWithCmd(DEFAULT_APP_PATH, std::string(DEFAULT_OTA_SAVE_PATH) + APP_NAME);
    Utility::startApp(std::string(DEFAULT_OTA_SAVE_PATH) + UPDATE_REPLACE_SCRIPT_NAME, false);
}

void OtaUnzipPkg(std::string pkgName)
{
    COUT << "Unzip Ota package" << endl;
    Utility::unzipFile(pkgName, DEFAULT_OTA_SAVE_PATH);
    Utility::changeFileMode(std::string(DEFAULT_OTA_SAVE_PATH) + APP_NAME, DEFAULT_APP_RIGHTS);
    Utility::changeFileMode(std::string(DEFAULT_OTA_SAVE_PATH) + RESTORE_SCRIPT_NAME, DEFAULT_APP_RIGHTS);
    Utility::changeFileMode(std::string(DEFAULT_OTA_SAVE_PATH) + UPDATE_REPLACE_SCRIPT_NAME, DEFAULT_APP_RIGHTS);
}

void RestartApp()
{
    COUT << "==================RestartApp==================" << endl;
    int tryTime = 0;
    bool bLaunched = false;
    while (tryTime++ < MAX_LAUNCH_TRY_TIME)
    {
        COUT << "==================RestartApp==================" << tryTime << endl;
        bLaunched = Utility::startApp(DEFAULT_APP_PATH, true);
        if (bLaunched)
        {
            COUT << "ControlBox 启动成功" << endl;
            break;
        }
        else
        {
            COUT << "ControlBox 启动失败" << endl;
        }
        sleep(1);
    }
    if (!bLaunched)
    {
        OtaRecovery();
    }
}

int DoOTA(std::string json)
{
    // 解析JSON字符串
    rapidjson::Document document;
    document.Parse(json.c_str());

    // 检查解析是否成功
    if (!document.IsObject())
    {
        COUT << "response解析失败！" << std::endl;
        return 0;
    }

    // 从JSON中获取值
    std::string status = document["status"].GetString();
    std::string needUpdate = document["needUpdate"].GetString();
    std::string url = document["url"].GetString();
    std::string md5 = document["md5"].GetString();
    std::string newVer = document["newVer"].GetString();

    // 打印获取的值
    COUT << "状态: " << status << std::endl;
    COUT << "是否有新版本: " << needUpdate << std::endl;
    COUT << "URL: " << url << std::endl;
    COUT << "MD5: " << md5 << std::endl;
    COUT << "newVer: " << newVer << std::endl;
    if (needUpdate == "true")
    {
        COUT << "Has new version" << std::endl;
        Utility::deleteDirectory(DEFAULT_OTA_SAVE_PATH);
        Utility::deleteDirectory(DEFAULT_OTA_BACKUP_PATH);
        // 创建升级包保存位置ota_save
        Utility::createDirIfNotExist(DEFAULT_OTA_SAVE_PATH);
        // 创建原程序备份位置ota_backup
        Utility::createDirIfNotExist(DEFAULT_OTA_BACKUP_PATH);

        std::string outputFile = DEFAULT_OTA_SAVE_PATH + Utility::getFilenameFromUrl(url);
        int downloadRes = HttpUtility::httpdownload(url, outputFile);
        if (downloadRes == CURLE_OK)
        {
            COUT << "File downloaded to: " << outputFile << std::endl;
            sleep(1);
            std::string otaMd5 = Utility::calculateMD5(outputFile);
            COUT << "Md5 checking" << md5 << std::endl;
            if (md5 != otaMd5)
            {
                COUT << "Invalid Md5" << std::endl;
                return 0;
            }
            // 备份原有版本到ota_backup
            OtaBackup();
            // 解压升级包，修改权限
            OtaUnzipPkg(outputFile);
            // 替换新文件
            OtaReplace();
            // sleep(5);
            RestartApp();
        }
    }
    else
    {
        COUT << "No new version" << std::endl;
    }
    return 1;
}

void OtaCheck()
{
    COUT << "Checking OTA" << i++ << std::endl;
    std::string strVer = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_VERSION_PATH));
    std::string deviceSN = Utility::removeTrailingNewline(Utility::getFileContent(DEFAULT_SN_FILE_PATH));
    // std::string deviceSN = DEVICE_SN;

    std::map<std::string, std::string> mapParam = {
        {"cmd", "otacheck"},
        {"version", strVer},
        {"sn", DEVICE_SN}};

    std::string strParam = HttpUtility::buildQueryString(mapParam);
    // COUT << "====check params====" << strParam << endl;
    std::string response;
    CURLcode getRes = HttpUtility::httpget(URL_CHECK_OTA, strParam, response, 1000);
    if (getRes == CURLE_OK)
    {
        COUT << "Get request successful" << std::endl;
        COUT << "Response: " << response << std::endl;
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
        COUT << "log请求response解析失败！" << endl;
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
    COUT << "Checking if need to upload log" << i++ << std::endl;
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
    COUT << "=========OTAUPDATER=============" << VERSION << endl;
    int index = 0;
    // OtaCheck();
    LogCheck();
    while (true)
    {
        //     if (index % 10 == 0)
        //         OtaCheck();
        //     else
        //         LogCheck();
        //     // std::this_thread::sleep_for(std::chrono::hours(1));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        COUT << VERSION << "---------------" << index++ << endl;
    }
}
