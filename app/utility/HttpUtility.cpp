#include "HttpUtility.h"

size_t HttpUtility::write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string data((const char *)ptr, (size_t)size * nmemb);
    *((std::string *)stream) += data;
    return size * nmemb;
}

size_t HttpUtility::write_callback_download(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ofstream *file = (std::ofstream *)stream;
    file->write((const char *)ptr, size * nmemb);
    return size * nmemb;
}

CURLcode HttpUtility::httpget(const std::string &url, const std::string &params, std::string &response, long timeout)
{
    std::string full_url = url + "?" + params;
    CURL *curl;
    CURLcode res;
    response = "";
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            if (res == CURLE_OPERATION_TIMEDOUT)
            {
                std::cout << "Request timed out" << std::endl;
            }
            else
            {
                std::cout << "Request failed: " << curl_easy_strerror(res) << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
    return res;
}

CURLcode HttpUtility::httpdownload(const std::string &url, const std::string &outputFile, long timeout)
{
    CURL *curl = nullptr;
    CURLcode res;
    std::ofstream file(outputFile, std::ios::binary);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_download);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            if (res == CURLE_OPERATION_TIMEDOUT)
            {
                std::cout << "Download timed out" << std::endl;
            }
            else
            {
                std::cout << "Download failed: " << curl_easy_strerror(res) << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    file.close();
    return res;
}
