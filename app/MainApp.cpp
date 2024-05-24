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
// #include "Base.h"

// size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
// {
//     std::ofstream *file = (std::ofstream *)stream;
//     file->write((const char *)ptr, size * nmemb);
//     return size * nmemb;
// }

// void httpdownload(const std::string &url, const std::string &outputFile)
// {
//     CURL *curl;
//     CURLcode res;
//     std::ofstream file(outputFile, std::ios::binary);

//     curl = curl_easy_init();
//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//     }

//     file.close();
// }

// size_t write_callback_get(void *ptr, size_t size, size_t nmemb, void *stream)
// {
//     std::string data((const char *)ptr, (size_t)size * nmemb);
//     *((std::string *)stream) += data;
//     return size * nmemb;
// }

// std::string httpget(const std::string &url, const std::string &params)
// {
//     std::string full_url = url + "?" + params;
//     CURL *curl;
//     CURLcode res;
//     std::string response;

//     curl = curl_easy_init();
//     if (curl)
//     {
//         curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_get);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//     }

//     return response;
// }

int main()
{
    std::string url = "http://192.168.80.235:8000/otadd/good.zip";
    std::string params = "key=value";
    std::string outputFile = "good.zip";
    std::string response;

    CURLcode getRes = HttpUtility::httpget("http://192.168.80.235:8000/otacheck", "", response);
    if (getRes == CURLE_OK)
    {
        std::cout << "Get request successful" << std::endl;
        std::cout << "Response: " << response << std::endl;
    }

    CURLcode downloadRes = HttpUtility::httpdownload(url, outputFile);
    if (downloadRes == CURLE_OK)
    {
        std::cout << "File downloaded to: " << outputFile << std::endl;
    }

    return 0;
}
