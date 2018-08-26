#ifndef __CURLWRAPPER_H
#define __CURLWRAPPER_H

#include <curl/curl.h>

class CurlWrapper
{
public:
    CurlWrapper();
    ~CurlWrapper();

    bool request(const std::string& request, std::string& response);
private:
    CURL* m_curl;
};
#endif // !__CURLWRAPPER_H

