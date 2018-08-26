#include "stdafx.h"
#include "Server.h"

#include "Acceptor.h"

Server::Server() 
{
	m_work.reset(new boost::asio::io_service::work(m_ios));
}

Server::~Server()
{
}

void Server::start(unsigned short port_num,	unsigned int thread_pool_size) 
{
	assert(thread_pool_size > 0);

	acc.reset(new Acceptor(m_ios, port_num));
	acc->start();

	for (unsigned int i = 0; i < thread_pool_size; i++) 
	{
		std::unique_ptr<std::thread> th(new std::thread([this]()
		{
			m_ios.run();
		}));

		m_thread_pool.push_back(std::move(th));
	}
}

void Server::stop() 
{
	acc->stop();
	m_ios.stop();

	for (auto& thread : m_thread_pool) 
	{
		thread->join();
	}
}
