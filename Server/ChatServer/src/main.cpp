#include "ChatServer.h"
#include "ConfigManager.h"

int main(int argc, char* argv[]) {
	ConfigManager::getInstance()->load();
	uint16_t port = ConfigManager::getInstance()->port();

	ChatServer server(port);
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