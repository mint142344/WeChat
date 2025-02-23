#pragma once

#include "Singleton.hpp"

#include <cstdint>
#include <string>

class ConfigManager : public Singleton<ConfigManager> {
	friend class Singleton<ConfigManager>;

public:
	ConfigManager() = default;
	~ConfigManager() = default;
	ConfigManager(const ConfigManager&) = delete;
	ConfigManager(ConfigManager&&) = delete;
	ConfigManager& operator=(const ConfigManager&) = delete;
	ConfigManager& operator=(ConfigManager&&) = delete;

	void load(const std::string& path);

private:
	void createDefaultConfig(const std::string& path);

public:
	// gate server
	std::string m_gate_ip;
	uint16_t m_gate_port;
	uint32_t m_asio_io_context_pool_size;

	// rpc
	std::string m_rpc_host;
	uint16_t m_rpc_port;
	uint32_t m_rpc_service_conn_pool_size;

	// redis
	std::string m_redis_host;
	uint16_t m_redis_port;
	uint32_t m_redis_conn_pool_size;

	// mysql
	std::string m_mysql_host;
	uint16_t m_mysql_port;
	std::string m_mysql_user;
	std::string m_mysql_password;
	uint32_t m_mysql_conn_pool_size;
};