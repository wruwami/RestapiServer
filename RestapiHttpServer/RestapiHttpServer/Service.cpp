#include "stdafx.h"
#include "Error.h"
#include "Service.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

Service::Service(std::shared_ptr<boost::asio::ip::tcp::socket>& sock) 
    : m_sock(sock)
    , m_request(4096)
{
}

Service::~Service()
{
}

void Service::start()
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
        Error::printErrorCode(ec);
        if (ec == boost::asio::error::not_found) 
        {
            sendResponse(Error::HTTP_NOT_FOUND);
        }
        else 
        {
            finish();
        }
        return;
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
        sendResponse(Error::HTTP_NOT_IMPLEMENTED);
        return;
    }

    request_line_stream >> m_requested_resource;
    m_requested_resource.erase(m_requested_resource.begin());

    std::cout << m_requested_resource.c_str() << std::endl;

    std::string request_http_version;
    request_line_stream >> request_http_version;
    if (request_http_version != "HTTP/1.1") 
    {
        sendResponse(Error::HTTP_VERSION_NOT_SUPPORTED);
        return;
    }

    boost::asio::async_read_until(*m_sock.get(), m_request, "\r\n", 
        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        onHeadersReceived(ec, bytes_transferred);
    });
}

void Service::onHeadersReceived(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) 
    {
        Error::printErrorCode(ec);

        if (ec == boost::asio::error::not_found) 
        {
            sendResponse(Error::HTTP_NOT_FOUND);
        }
        else 
        {
            finish();
        }
        return;
    }

    try
    {
        processRequest();
        sendResponse(Error::HTTP_OK);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        m_resource_buffer.clear();
        sendResponse(Error::HTTP_SERVER_ERROR);
    }
}

void Service::processRequest()
{
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

    m_resource_buffer.erase(m_resource_buffer.rfind(','));
    m_resource_buffer += std::string("]");
    std::cout << "response : " + m_resource_buffer << std::endl;
}

void Service::sendResponse(unsigned int response_status_code)
{
    m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
    std::string status_line = Error::http_status_table.at(response_status_code);
    m_status_line_buffer = std::string("HTTP/1.1 ") + status_line + "\r\n";
    m_response_headers += "\r\n\r\n";
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

    boost::asio::async_write(*m_sock.get(), response_buffers,
         [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
     {
         onResponseSend(ec, bytes_transferred);
     });
}

void Service::onResponseSend(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) 
    {
        Error::printErrorCode(ec);
    }

    m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    finish();
}

void Service::finish()
{
    delete this;
}