#ifndef __SERVICE_H
#define __SERVICE_H

#include <boost/asio.hpp>

class Service
{
public:
	Service(std::shared_ptr<boost::asio::ip::tcp::socket>& sock);
	virtual ~Service();

	void start();

private:
	void onRequestLineReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void onHeadersReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);

	virtual void processRequest();
	virtual void sendResponse(unsigned int response_status_code);
	void onResponseSend(const boost::system::error_code& ec, std::size_t bytes_transferred);

    void finish();

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	boost::asio::streambuf m_request;
	std::string m_requested_resource;

    std::string m_status_line_buffer;
	std::string m_response_headers;
    std::string m_resource_buffer;
};
#endif // __SERVICE_H