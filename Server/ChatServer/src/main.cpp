#include "ChatServer.h"
#include "ConfigManager.h"
#include "RpcService.h"

int main(int argc, char* argv[]) {
	ConfigManager::getInstance()->load();
	auto* config = ConfigManager::getInstance();

	// 1. Init RPC Service
	RpcServiceConnPool<StatusService>::getInstance()->init(
		config->statusHost(), config->statusPort(), config->statusConnPoolSize());

	// 2. Start ChatServer
	ChatServer server(config->port());
	if (!server.start()) return 1;

	try {
		while (server.isRunning()) {
			server.handleMessage();
		}
	} catch (std::exception& e) {
		fmt::println(stderr, "exception: {}", e.what());
	}

	return 0;
}