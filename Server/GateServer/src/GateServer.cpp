#include "ConfigManger.h"
#include "pool/RpcConnPool.hpp"
#include "pool/IoContextPool.h"
#include "pool/MysqlConnPool.h"
#include "pool/RedisConnPool.h"
#include "LogicSystem.h"
#include "MysqlService.h"
#include "Listener.h"

#include <fmt/base.h>
#include <csignal>
#include <memory>

int main(int argc, char* argv[]) {
	// argc argv 指定其他路径的配置文件, 生成 config example
	if (argc == 2) {
		ConfigManager::genDefaultConfig("example_config.json");
		return 0;
	}

	net::io_context ioc;
	net::signal_set signals(ioc, SIGINT, SIGTERM);

	try {
		// 1.load config
		ConfigManager::getInstance()->load();
		const ConfigManager* config = ConfigManager::getInstance();

		tcp::endpoint ep({net::ip::make_address(config->m_gate_ip), config->m_gate_port});

		// 2.start io_context pool
		IoContextPool::getInstance()->start(config->m_io_context_pool_size);

		// 3.init logic system
		LogicSystem::getInstance()->init();

		// 4.init RPC pool
		RpcServiceConnPool<EmailVerifyService>::getInstance()->init(
			config->m_rpc_email_host, config->m_rpc_email_port,
			config->m_rpc_email_service_pool_size);
		RpcServiceConnPool<StatusService>::getInstance()->init(
			config->m_rpc_status_host, config->m_rpc_status_port,
			config->m_rpc_status_service_pool_size);

		// 5.init redis conn pool
		RedisConnPool::getInstance()->init(config->m_redis_host, config->m_redis_port,
										   config->m_redis_conn_pool_size);

		// 6.init database conn pool
		MysqlConnPool::getInstance()->init(config->m_mysql_host, config->m_mysql_port,
										   config->m_mysql_user, config->m_mysql_password,
										   config->m_mysql_db, config->m_mysql_conn_pool_size);
		// 7.init mysql service
		MysqlService::getInstance()->init();

		// 8.register signal & gracefully quit
		signals.async_wait([&ioc](const std::error_code& ec, int signal) {
			if (ec) {
				fmt::print(stderr, "signal error: {}\n", ec.message());
				return;
			}
			ioc.stop();
			fmt::println("Interrupted by a signal {}", signal);
		});

		// 9.start gate server
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