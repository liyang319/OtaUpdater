#ifndef HTTPUTILITY_H
#define HTTPUTILITY_H

#include <curl/curl.h>
#include <iostream>
#include <fstream>

class HttpUtility
{
public:
    static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t write_callback_download(void *ptr, size_t size, size_t nmemb, void *stream);
    static CURLcode httpget(const std::string &url, const std::string &params, std::string &response, long timeout = 30);
    static CURLcode httpdownload(const std::string &url, const std::string &outputFile, long timeout = 30);
    static bool urlExists(const std::string &url);
};

#endif
