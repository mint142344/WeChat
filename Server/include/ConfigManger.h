#pragma once

#include "Singleton.hpp"

#include <cstdint>
#include <string>

class ConfigManager : public Singleton<ConfigManager> {
	friend class Singleton<ConfigManager>;

public:
	~ConfigManager() = default;
	void load(const std::string& path);

private:
	void createDefaultConfig(const std::string& path);

public:
	std::string m_gate_ip;
	uint16_t m_gate_port;

	std::string m_email_rpc_host;
	uint16_t m_email_rpc_port;

	std::string m_redis_host;
	uint16_t m_redis_port;

	uint32_t m_asio_io_context_pool_size;
	uint32_t m_rpc_service_conn_pool_size;
};