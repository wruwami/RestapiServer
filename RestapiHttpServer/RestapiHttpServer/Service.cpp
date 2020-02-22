#include "stdafx.h"
#include "Service.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>

#define USING_BOOST_JSON_PARSER

#if !defined(USING_BOOST_JSON_PARSER)
#define USING_JSON_CPP_PARSER
#endif // USING_BOOST_JSON_PARSER

#if defined(USING_JSON_CPP_PARSER)
#include <json\json.h>
#elif defined(USING_BOOST_JSON_PARSER)
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#endif // USING_JSON_CPP_PARSER

enum HttpResponseCode
{
    HTTP_OK = 200,
    HTTP_NOT_FOUND = 404,
    HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
    HTTP_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505,
};

std::map<unsigned int, std::string> http_status_table =
{
    { HTTP_OK, "200 OK" },
    { HTTP_NOT_FOUND, "404 Not Found" },
    { HTTP_REQUEST_ENTITY_TOO_LARGE, "413 Request Entity Too Large" },
    { HTTP_SERVER_ERROR, "500 Server Error" },
    { HTTP_NOT_IMPLEMENTED, "501 Not Implemented" },
    { HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported" }
};


Service::Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) 
    : m_sock(sock)
    , m_request(4096)
{
}

Service::~Service()
{
}

void Service::startHandling()
{
    boost::asio::async_read_until(*m_sock.get(), m_request, "\r\n",
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        onRequestLineReceived(ec, bytes_transferred);
    });
}

void Service::onRequestLineReceived(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) 
    {
        printErrorCode(ec);
        if (ec == boost::asio::error::not_found) 
        {
            sendResponse(HTTP_REQUEST_ENTITY_TOO_LARGE);
            return;
        }
        else 
        {
            onFinish();
            return;
        }
    }

    std::string request_line;
    std::istream request_stream(&m_request);
    std::getline(request_stream, request_line, '\r');
    request_stream.get();

    std::string request_method;
    std::istringstream request_line_stream(request_line);
    request_line_stream >> request_method;
    if (request_method != "GET") 
    {
        sendResponse(HTTP_NOT_IMPLEMENTED);
        return;
    }

    request_line_stream >> m_requested_resource;
    m_requested_resource.erase(m_requested_resource.begin());

    std::cout << m_requested_resource.c_str() << std::endl;

    std::string request_http_version;
    request_line_stream >> request_http_version;

    if (request_http_version != "HTTP/1.1") 
    {
        sendResponse(HTTP_VERSION_NOT_SUPPORTED);
        return;
    }

    boost::asio::async_read_until(*m_sock.get(), m_request, "\r\n", [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        onHeadersReceived(ec, bytes_transferred);
    });
}

void Service::onHeadersReceived(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) 
    {
        printErrorCode(ec);

        if (ec == boost::asio::error::not_found) 
        {
            sendResponse(HTTP_REQUEST_ENTITY_TOO_LARGE);
        }
        else 
        {
            onFinish();
        }
        return;
    }

    try
    {
        processRequest();
        sendResponse(HTTP_OK);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        m_resource_buffer.clear();
        sendResponse(HTTP_NOT_FOUND);
    }
}

void Service::processRequest()
{
#if defined(USING_JSON_CPP_PARSER)
    Json::Value value;
    std::ifstream fs("..\\db.json", std::ifstream::binary);
    fs >> value;
    m_resource_buffer += std::string("[");
    for (const auto& v : value[m_requested_resource])
    {
        m_resource_buffer += std::string("{ ");
        for (const auto& id : v.getMemberNames())
        {
            //std::cout << id << ": " << v[id] << std::endl;
            m_resource_buffer += "\"" + boost::lexical_cast<std::string>(id) + "\": " + boost::lexical_cast<std::string>(v[id]) + ",";
        }
        m_resource_buffer.erase(m_resource_buffer.rfind(','));
        m_resource_buffer += std::string("}, ");
    }

#elif defined(USING_BOOST_JSON_PARSER)
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("..\\db.json", pt);

    m_resource_buffer += std::string("[");
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child(m_requested_resource.c_str()))
    {
        assert(v.first.empty());
        if (m_requested_resource == "todos")
        {
            int id = v.second.get<int>("id");
            std::string todo = v.second.get<std::string>("todo");
            m_resource_buffer += std::string("{ \"id\": ") + boost::lexical_cast<std::string>(id) + std::string(", \"todo\":") + todo + std::string("} ,");
        }
        else if (m_requested_resource == "users")
        {
            int id = v.second.get<int>("id");
            std::string name = v.second.get<std::string>("name");
            int age = v.second.get<int>("age");
            std::string gender = v.second.get<std::string>("gender");
            m_resource_buffer += std::string("{ \"id\": ") + boost::lexical_cast<std::string>(id)
                + std::string(", \"name\":") + name
                + std::string(", \"age\": ") + boost::lexical_cast<std::string>(age)
                + std::string(", \"gender\": ") + gender + std::string("} ,");
        }
        else if (m_requested_resource == "hobbies")
        {
            int id = v.second.get<int>("id");
            std::string hobby = v.second.get<std::string>("hobby");
            m_resource_buffer += std::string("{ \"id\": ") + boost::lexical_cast<std::string>(id)
                + std::string(", \"hobby\":") + hobby + std::string("} ,");
        }
    }
#endif // USING_JSON_CPP_PARSER

    m_resource_buffer.erase(m_resource_buffer.rfind(','));
    m_resource_buffer += std::string("]");
    std::cout << "response : " + m_resource_buffer << std::endl;
}

void Service::sendResponse(unsigned int response_status_code)
{
    m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
    std::string status_line = http_status_table.at(response_status_code);
    m_status_line_buffer = std::string("HTTP/1.1 ") + status_line + "\r\n";
    m_response_headers += "\r\n";
    std::vector<boost::asio::const_buffer> response_buffers;
    response_buffers.push_back(boost::asio::buffer(m_status_line_buffer));

    if (m_response_headers.length() > 0) 
    {
        response_buffers.push_back(boost::asio::buffer(m_response_headers));
    }

    if (m_resource_buffer.length() > 0)
    {
        response_buffers.push_back(boost::asio::buffer(m_resource_buffer));
    }

//    boost::asio::async_write(*m_sock.get(), response_buffers,
//         [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
//     {
//         onResponseSend(ec, bytes_transferred);
//     });
}

void Service::onResponseSend(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) 
    {
        printErrorCode(ec);
    }

    m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    onFinish();
}

void Service::onFinish()
{
    delete this;
}

void Service::printErrorCode(const boost::system::error_code &ec)
{
    std::cout << "Error occurred! Error code = " << ec.value() << ". Message: " << ec.message();
}