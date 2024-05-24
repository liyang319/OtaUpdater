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

int i = 0;
void OtaCheck()
{
    std::cout << "=========OtaCheck=======" << i++ << std::endl;
    string strVer = Utility::getFileContent(DEFAULT_VERSION_PATH);
    // cout << "----------" << strVer << "----" << strVer.length() << endl;

    // Document document;
    // document.SetObject();
    // // Add values to the document
    // rapidjson::Value verVal;
    // verVal.SetString(strVer.c_str(), document.GetAllocator());
    // // rapidjson::Value snVal;
    // // verVal.SetString(strVer.c_str(), document.GetAllocator());

    // document.AddMember("cmd", "otacheck", document.GetAllocator());
    // document.AddMember("version", verVal, document.GetAllocator());
    // document.AddMember("sn", DEVICE_SN, document.GetAllocator());
    // // Convert the document to a string
    // StringBuffer buffer;
    // Writer<StringBuffer> writer(buffer);
    // document.Accept(writer);
    // // Print the JSON string
    // std::cout << buffer.GetString() << std::endl;

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
    }
}

int main()
{
    while (true)
    {
        OtaCheck();
        // std::this_thread::sleep_for(std::chrono::hours(1));
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}

// std::string url = "http://192.168.80.235:8000/ota/good.zip";
// std::string params = "key=value";
// std::string outputFile = "good.zip";
// std::string response;

// CURLcode getRes = HttpUtility::httpget("http://192.168.80.235:8000/otacheck", "", response);
// if (getRes == CURLE_OK)
// {
//     std::cout << "Get request successful" << std::endl;
//     std::cout << "Response: " << response << std::endl;
// }

// CURLcode downloadRes = HttpUtility::httpdownload(url, outputFile);
// if (downloadRes == CURLE_OK)
// {
//     std::cout << "File downloaded to: " << outputFile << std::endl;
// }