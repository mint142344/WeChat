#pragma once
#include "Common/net.h"
#include <memory>

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
public:
	HttpConnection(const HttpConnection&) = delete;
	HttpConnection(HttpConnection&&) = delete;
	HttpConnection& operator=(const HttpConnection&) = delete;
	HttpConnection& operator=(HttpConnection&&) = delete;

	HttpConnection(net::io_context& ioc, tcp::socket socket);

	~HttpConnection() {
		// auto ep = m_socket.remote_endpoint();
		// fmt::println("{}:{} disconnected", ep.address().to_string(), ep.port());
	}

	void start();

private:
	// [ASYNC]
	void startTimeoutCheck(int seconds = 30);
	void on_read(size_t length);
	void on_write(size_t length);

private:
	net::io_context& m_ioc;
	tcp::socket m_socket;

	beast::flat_buffer m_buf;
	http::request<http::string_body> m_req;
	http::response<http::dynamic_body> m_res;

	net::steady_timer m_timer;
};