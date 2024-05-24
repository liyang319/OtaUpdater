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
// #include "Base.h"

using namespace rapidjson;

int main()
{

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