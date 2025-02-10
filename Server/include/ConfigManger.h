#pragma once

#include "Singleton.hpp"

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
};