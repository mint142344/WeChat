#pragma once
#include "net.h"

// 同步客户端
class TcpTester {
public:
	explicit TcpTester(net::io_context& ctx) : m_ctx(ctx), m_socket(ctx) {}

	// 主机名解析
	bool canResolveHost(const std::string& host, const uint16_t port) {
		tcp::resolver resolver(m_ctx);
		boost::system::error_code ec;
		auto eps = resolver.resolve(host, std::to_string(port), ec);

		return !ec;
	}

	// 测试连接
	bool testConnect(const std::string& host, const uint16_t port) {
		tcp::resolver resolver(m_ctx);
		boost::system::error_code ec;
		auto eps = resolver.resolve(host, std::to_string(port), ec);

		if (ec) {
			return false;
		}

		net::connect(m_socket, eps, ec);

		return !static_cast<bool>(ec);
	}

private:
	net::io_context& m_ctx;
	tcp::socket m_socket;
};