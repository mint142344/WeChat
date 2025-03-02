#include "ConfigManager.h"

#include <fstream>
#include <string>

void ConfigManager::load(const std::string& path) {
	std::ifstream ifs(path);
	if (!ifs) {
		throw std::runtime_error(path + " file not found");
	}

	m_config = json::parse(ifs);
	std::string error;
	if (!checkChatServerConfig(m_config, error)) {
		throw std::runtime_error(error + "\nPlease refer to the format below\n" +
								 DEFAULT_CHAT_SERVER_CONFIG.dump(4));
	}
}

uint16_t ConfigManager::port() const { return m_config["ChatServer"]["port"].get<uint16_t>(); }

size_t ConfigManager::maxConnections() const {
	return m_config["ChatServer"]["max_connections"].get<size_t>();
}

size_t ConfigManager::beatTimeout() const {
	return m_config["ChatServer"]["beat_timeout"].get<size_t>();
}

size_t ConfigManager::checkBeatTime() const {
	return m_config["ChatServer"]["check_beat_time"].get<size_t>();
}

bool ConfigManager::checkChatServerConfig(const json& data, std::string& error) {
	if (!data.contains("ChatServer")) {
		error = "required ChatServer Object";
		return false;
	}

	const auto& chat = data["ChatServer"];
	if (!chat.contains("port") || !chat.contains("max_connections") ||
		!chat.contains("beat_timeout") || !chat.contains("check_beat_time")) {
		error =
			"required ChatServer.port, ChatServer.max_connections, ChatServer.beat_timeout, "
			"ChatServer.check_beat_time";
		return false;
	}

	return true;
}
