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

	m_acc.reset(new Acceptor(m_ios, port_num));
	m_acc->start();

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
	m_ios.stop();
	m_acc->stop();

	for (auto& thread : m_thread_pool) 
	{
		thread->join();
	}
}
