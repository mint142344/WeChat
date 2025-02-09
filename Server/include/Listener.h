#pragma once

#include "Common.h"

class Listener : public std::enable_shared_from_this<Listener> {
public:
	Listener(net::io_context& ioc, tcp::endpoint endpoint);

	void start();

private:
	void on_accept(tcp::socket socket);

private:
	net::io_context& m_ioc;
	tcp::acceptor m_acceptor;
};