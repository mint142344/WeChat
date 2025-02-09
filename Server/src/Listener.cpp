#include "Listener.h"
#include "HttpConnection.h"
#include <fmt/base.h>

Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint)
	: m_ioc(ioc), m_acceptor(ioc, endpoint) {}

void Listener::start() {
	m_acceptor.async_accept(
		[self = shared_from_this()](const std::error_code& ec, tcp::socket socket) {
			if (ec) {
				fmt::println("accept:{}", ec.message());
				return;
			}

			self->on_accept(std::move(socket));
		});
}

void Listener::on_accept(tcp::socket socket) {
	// start a http connection
	std::make_shared<HttpConnection>(m_ioc, std::move(socket))->start();

	// continue to accept
	start();
}
