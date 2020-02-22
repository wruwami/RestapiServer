#include "stdafx.h"

#include <boost\asio.hpp>

#include "Acceptor.h"
#include "Service.h"

Acceptor::Acceptor(boost::asio::io_service& ios, unsigned short port_num)
    : m_ios(ios)
    , m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_num))
    , m_is_stopped(false)
{
}

Acceptor::~Acceptor()
{
}

void Acceptor::start()
{
    m_acceptor.listen();
    init();
}

void Acceptor::stop()
{
    m_is_stopped.store(true);
}

void Acceptor::init()
{
    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(m_ios));

    m_acceptor.async_accept(*sock.get(), [this, sock](const boost::system::error_code& error)
    {
        onAccept(error, sock);
    });
}

void Acceptor::onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
    if (ec.value() == 0)
    {
        (new Service(sock))->startHandling();
    }
    else
    {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }

    if (!m_is_stopped.load())
    {
        init();
    }
    else
    {
        m_acceptor.close();
    }
}
