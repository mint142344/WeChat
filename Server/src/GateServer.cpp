#include "Listener.h"
#include "ConfigManger.h"

#include <fmt/base.h>
#include <csignal>
#include <memory>

int main(int argc, char* argv[]) {
	net::io_context ioc;
	net::signal_set signals(ioc, SIGINT, SIGTERM);

	try {
		// 读取配置文件
		ConfigManager::getInstance()->load("config.json");
		tcp::endpoint ep({net::ip::make_address(ConfigManager::getInstance()->m_gate_ip),
						  ConfigManager::getInstance()->m_gate_port});

		signals.async_wait([&ioc](const std::error_code& ec, int signal) {
			ioc.stop();
			fmt::println("Interrupted by a signal {}", signal);
		});

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