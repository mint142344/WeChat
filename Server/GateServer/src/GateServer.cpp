#include "ConfigManger.h"
#include "LogicSystem.h"
#include "Listener.h"

#include "pool/IoContextPool.h"
#include "pool/MysqlConnPool.h"
#include "pool/RedisConnPool.h"

#include "MysqlService.h"
#include "RpcService.h"

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

		tcp::endpoint ep({net::ip::make_address(config->gateIp()), config->gatePort()});

		// 2.start io_context pool
		IoContextPool::getInstance()->start(config->ioContextPoolSize());

		// 3.init logic system
		LogicSystem::getInstance()->init();

		// 4.init RPC pool
		RpcServiceConnPool<EmailVerifyService>::getInstance()->init(
			config->rpcEmailHost(), config->rpcEmailPort(), config->rpcEmailServicePoolSize());
		RpcServiceConnPool<StatusService>::getInstance()->init(
			config->rpcStatusHost(), config->rpcStatusPort(), config->rpcStatusServicePoolSize());

		// 5.init redis conn pool
		RedisConnPool::getInstance()->init(config->redisHost(), config->redisPort(),
										   config->redisConnPoolSize());

		// 6.init database conn pool
		MysqlConnPool::getInstance()->init(config->mysqlHost(), config->mysqlPort(),
										   config->mysqlUser(), config->mysqlPassword(),
										   config->mysqlDb(), config->mysqlConnPoolSize());
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