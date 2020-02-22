#ifndef __SERVER_H
#define __SERVER_H

#include <boost/asio.hpp>

class Acceptor;

class Server
{
public:
    Server();
    virtual ~Server();

    void start(unsigned short port_num, unsigned int thread_pool_size);
    void stop();

private:
    boost::asio::io_service m_ios;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> m_acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
};
#endif // !__SERVER_H