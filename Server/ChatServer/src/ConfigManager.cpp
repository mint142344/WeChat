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

size_t ConfigManager::checkBeatDuration() const {
	return m_config["ChatServer"]["beat_duration"].get<size_t>();
}

std::string ConfigManager::statusHost() const {
	return m_config["RPC"][0]["StatusService"]["host"].get<std::string>();
}

uint16_t ConfigManager::statusPort() const {
	return m_config["RPC"][0]["StatusService"]["port"].get<uint16_t>();
}

uint16_t ConfigManager::statusConnPoolSize() const {
	return m_config["RPC"][0]["StatusService"]["conn_pool_size"].get<uint16_t>();
}

bool ConfigManager::checkChatServerConfig(const json& data, std::string& error) {
	// ChatServer
	if (!data.contains("ChatServer")) {
		error = "required ChatServer Object";
		return false;
	}

	const auto& chat = data["ChatServer"];
	if (!chat.contains("port") || !chat.contains("max_connections") ||
		!chat.contains("beat_timeout") || !chat.contains("beat_duration")) {
		error =
			"required ChatServer.port, ChatServer.max_connections, ChatServer.beat_timeout, "
			"ChatServer.beat_duration";
		return false;
	}

	// RPC Service
	if (!data.contains("RPC") || !data["RPC"].is_array()) {
		error = "required RPC Array";
		return false;
	}

	// RPC[0] StatusService
	const auto& rpc = data["RPC"];
	if (rpc.size() < 1 || !rpc[0].contains("StatusService")) {
		error = "required RPC[0].StatusService Object";
		return false;
	}

	const auto& status = rpc[0]["StatusService"];
	if (!status.contains("conn_pool_size") || !status.contains("host") ||
		!status.contains("port")) {
		error = "required StatusService.conn_pool_size, StatusService.host, StatusService.port";
		return false;
	}

	return true;
}
