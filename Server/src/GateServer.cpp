#include "EmailVerifyClient.h"
#include "Listener.h"
#include "ConfigManger.h"
#include "IoContextPool.h"
#include "EmailVerifyClient.h"

#include <fmt/base.h>
#include <csignal>
#include <memory>

int main(int argc, char* argv[]) {
	net::io_context ioc;
	net::signal_set signals(ioc, SIGINT, SIGTERM);

	try {
		// 1.load config
		ConfigManager::getInstance()->load("config.json");
		const ConfigManager* config = ConfigManager::getInstance();

		tcp::endpoint ep({net::ip::make_address(config->m_gate_ip), config->m_gate_port});

		// 2.start io_context pool
		IoContextPool::getInstance()->start(config->m_asio_io_context_pool_size);

		// 3.init RPC pool
		RpcServiceConnPool<EmailVerifyService>::getInstance()->init(
			config->m_email_rpc_host, config->m_email_rpc_port,
			config->m_rpc_service_conn_pool_size);

		// 4.register signal & gracefully quit
		signals.async_wait([&ioc](const std::error_code& ec, int signal) {
			ioc.stop();
			fmt::println("Interrupted by a signal {}", signal);
		});

		// 5.start gate server
		std::make_shared<Listener>(ioc, ep)->start();
		fmt::println("Listening on {}:{}", ep.address().to_string(), ep.port());

		ioc.run();

	} catch (std::exception& e) {
		fmt::println("Exception: {}", e.what());
		return 1;
	}

	ioc.run();

	return 0;
}