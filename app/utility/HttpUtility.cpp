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
    std::string full_url = url + params;
    // std::string full_url = "http://192.168.80.235:8000/otacheck?cmd=otacheck&sn=123456789&version=1.2.3";
    std::cout << full_url << std::endl;
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
    if (!urlExists(url))
    {
        std::cout << "URL does not exist" << std::endl;
        return CURLE_URL_MALFORMAT;
    }

    CURL *curl;
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

bool HttpUtility::urlExists(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Perform HEAD request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            return (response_code == 200);
        }
    }
    return false;
}

std::string HttpUtility::buildQueryString(std::map<std::string, std::string> &params)
{
    std::string queryParams = "";
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        if (queryParams.empty())
        {
            queryParams += "?";
        }
        else
        {
            queryParams += "&";
        }
        queryParams += it->first + "=" + it->second;
    }
    return queryParams;
}

size_t HttpUtility::writeCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    // FILE *file = (FILE *)userdata;
    // size_t written = fwrite(ptr, size, nmemb, file);
    // return written;
    ((std::string *)userdata)->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

int HttpUtility::httpUploadFile(std::string url, std::string filePath, std::string fileName)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *file = fopen(filePath.c_str(), "rb");
        if (file)
        {
            struct curl_httppost *formpost = NULL;
            struct curl_httppost *lastptr = NULL;
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, filePath.c_str(), CURLFORM_END);
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, fileName.c_str(), CURLFORM_END);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "Failed to upload file: " << curl_easy_strerror(res) << std::endl;
                curl_formfree(formpost);
                fclose(file);
                curl_easy_cleanup(curl);
                return 1;
            }
            curl_formfree(formpost);
            fclose(file);
            curl_easy_cleanup(curl);
            return 0;
        }
        else
        {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }
    }
    else
    {
        std::cerr << "Failed to initialize libcurl" << std::endl;
        return 1;
    }
}