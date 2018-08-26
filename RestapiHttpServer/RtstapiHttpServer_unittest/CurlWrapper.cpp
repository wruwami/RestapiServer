#include "stdafx.h"
#include "CurlWrapper.h"

#include <iostream>

const std::string BASE_URL = "http://localhost:3333/";

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) 
{
    data->append((char*)ptr, size * nmemb);
    //std::cout << *data << std::endl;
    return size * nmemb;
}

CurlWrapper::CurlWrapper()
    : m_curl(NULL)
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curl = curl_easy_init();
    
}

CurlWrapper::~CurlWrapper()
{
    if (m_curl)
    {
        curl_easy_cleanup(m_curl);
        curl_global_cleanup();
    }
}

bool CurlWrapper::request(const std::string& request, std::string& response)
{
    if (m_curl)
    {
        std::string url = BASE_URL + request;
        curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);

        std::string response_string;
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_string);

        std::cout << "request url : " << BASE_URL << request << std::endl;
        long response_code;
        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);

        CURLcode res = curl_easy_perform(m_curl);
        if (response_code == CURLE_OK)
        {
            response = response_string;
            return true;
        }
        else
        {
            std::cout << "curl_easy_perform() failed:" << curl_easy_strerror(res) << std::endl;
        }
    }

    return false;
}