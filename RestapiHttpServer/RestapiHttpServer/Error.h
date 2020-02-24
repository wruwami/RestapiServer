#ifndef ERROR_H
#define ERROR_H

#include <boost/asio.hpp>
#include <map>

namespace Error {
    enum HttpResponseCode
    {
        HTTP_OK = 200,
        HTTP_NOT_FOUND = 404,
        HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
        HTTP_SERVER_ERROR = 500,
        HTTP_NOT_IMPLEMENTED = 501,
        HTTP_VERSION_NOT_SUPPORTED = 505,
    };

    static std::map<unsigned int, std::string> http_status_table =
    {
        { HTTP_OK, "200 OK" },
        { HTTP_NOT_FOUND, "404 Not Found" },
        { HTTP_REQUEST_ENTITY_TOO_LARGE, "413 Request Entity Too Large" },
        { HTTP_SERVER_ERROR, "500 Server Error" },
        { HTTP_NOT_IMPLEMENTED, "501 Not Implemented" },
        { HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported" }
    };

    static void printErrorCode(const boost::system::system_error &e)
    {
        std::cout << "Error occurred! Error code = " << e.code() << ". Message: " << e.what();
    }

    static void printErrorCode(const boost::system::error_code &ec)
    {
        std::cout << "Error occurred! Error code = " << ec.value() << ". Message: " << ec.message();
    }
};

#endif // ERROR_H