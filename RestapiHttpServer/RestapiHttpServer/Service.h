#ifndef __SERVICE_H
#define __SERVICE_H

#include <boost/asio.hpp>

class Service
{
public:
	Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
	virtual ~Service();

	void start_handling();

private:
	void on_request_line_received(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_headers_received(const boost::system::error_code& ec, std::size_t bytes_transferred);

	virtual void process_request();
	virtual void send_response(unsigned int response_status_code);
	void on_response_sent(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_finish();

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	boost::asio::streambuf m_request;
	std::map<std::string, std::string> m_request_headers;
	std::string m_requested_resource;

	std::string m_resource_buffer;

	std::string m_response_headers;
	std::string m_response_status_line;

    const std::map<unsigned int, std::string> Service::http_status_table =
    {
        { 200, "200 OK" },
        { 404, "404 Not Found" },
        { 413, "413 Request Entity Too Large" },
        { 500, "500 Server Error" },
        { 501, "501 Not Implemented" },
        { 505, "505 HTTP Version Not Supported" }
    };
};
#endif // __SERVICE_H