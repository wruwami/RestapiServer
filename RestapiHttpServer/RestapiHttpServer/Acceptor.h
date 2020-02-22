#ifndef __ACCEPTOR_H
#define __ACCEPTOR_H

class Acceptor
{
public:
	Acceptor(boost::asio::io_service& ios, unsigned short port_num);
	virtual ~Acceptor();

	void start();
	void stop();

private:
	void init();
	void onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock);

private:
	boost::asio::io_service& m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_is_stopped;
};
#endif // __ACCEPTOR_H