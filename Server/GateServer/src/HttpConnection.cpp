#include "HttpConnection.h"
#include "LogicSystem.h"

#include <fmt/base.h>
#include <chrono>

HttpConnection::HttpConnection(net::io_context& ioc, tcp::socket socket)
	: m_ioc{ioc}, m_socket{std::move(socket)}, m_timer(ioc) {}

void HttpConnection::start() {
	// 对read 开启超时检测
	startTimeoutCheck();

	http::async_read(m_socket, m_buf, m_req,
					 [self = shared_from_this()](const std::error_code& ec, size_t length) {
						 if (ec) {
							 fmt::println("http request:{}", ec.message());
							 return;
						 }

						 // 关闭超时检测
						 self->m_timer.cancel();
						 // 处理请求
						 self->on_read(length);
					 });
}

void HttpConnection::startTimeoutCheck(int seconds) {
	m_timer.expires_after(std::chrono::seconds{seconds});

	m_timer.async_wait([self = shared_from_this()](const std::error_code& ec) {
		if (ec) {
			if (ec.value() != net::error::operation_aborted)
				fmt::println("http timer:{}", ec.message());
			return;
		}

		// 连接超时,关闭连接
		self->m_socket.close();
	});
}

void HttpConnection::on_read(size_t length) {
	m_res.version(m_req.version());
	// 短连接
	m_res.keep_alive(false);

	// 处理请求 response 交给 LogicSystem 处理
	LogicSystem::getInstance()->handleRequest(m_req.target(), m_req, m_res);

	// 对write 开启超时检测
	startTimeoutCheck();
	// 回复响应
	http::async_write(m_socket, m_res,
					  [self = shared_from_this()](const std::error_code& ec, size_t length) {
						  if (ec) {
							  fmt::println("http response:{}", ec.message());
							  return;
						  }
						  self->m_timer.cancel();
						  self->on_write(length);
					  });
}

void HttpConnection::on_write(size_t length) { m_socket.shutdown(tcp::socket::shutdown_send); }
