#pragma once

#include "Singleton.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// clang-format off
inline const json DEFAULT_CHAT_SERVER_CONFIG = {
	{"ChatServer", {
		{"port", 6002},
		{"max_connections", 1024},
		{"beat_timeout", 10},
		{"check_beat_time", 5}
	}}
};
// clang-format on

class ConfigManager : public Singleton<ConfigManager> {
	friend class Singleton<ConfigManager>;

public:
	~ConfigManager() = default;
	ConfigManager(const ConfigManager&) = delete;
	ConfigManager(ConfigManager&&) = delete;
	ConfigManager& operator=(const ConfigManager&) = delete;
	ConfigManager& operator=(ConfigManager&&) = delete;

	void load(const std::string& path = "chat_server_config.json");

	// port
	uint16_t port() const;

	// 最大连接数(max_connections)
	size_t maxConnections() const;

	// 心跳检测超时时间(beat_timeout)
	size_t beatTimeout() const;
	// 心跳检测周期(check_beat_time)
	size_t checkBeatTime() const;

private:
	static bool checkChatServerConfig(const json& data, std::string& error);

private:
	ConfigManager() = default;

private:
	json m_config;
};
